
#define FUNC_ONLY

#include <wdm.h>
#include "DriverEntry.h"
#include "DriverFunctions.h"
#include "ThreadLog.h"


extern "C"
NTSTATUS
DriverEntry(
	_DRIVER_OBJECT* DriverObject,
	PUNICODE_STRING RegistryPath
)
{
	PDEVICE_OBJECT			DeviceObject = nullptr;
	NTSTATUS				Status = STATUS_UNSUCCESSFUL;
	UNICODE_STRING			DeviceName, DosDeviceName = { 0 };

	UNREFERENCED_PARAMETER(RegistryPath);
	PAGED_CODE();

	DebugPrint("[KernelControl] Driver-Entry Funktion wurde geladen\n");
	DebugPrint("[KernelControl] DriverObject @ %p\n", DriverObject);

	RtlInitUnicodeString(&DeviceName, DEVICENAME);
	RtlInitUnicodeString(&DosDeviceName, DOSDEVICENAME);

	Status = IoCreateDevice (
		DriverObject,
		0,
		&DeviceName,
		FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN,
		FALSE,
		&DeviceObject		// übegebe Addresse für den Pointer, sodass die Addresse des
							// Objektes danach direkt in den Pointer geschrieben werden kann
	);

	// Wenn fehlgeschlagen, dann das Gerät löschen
	if (!NT_SUCCESS(Status))
	{
		DebugPrint("[KernelControl] Initialisation gescheitert. Code %u\n", Status);

		if (DeviceObject)
		{
			// Es kann auch sein, dass DeviceObject NULL ist. DeviceObject ist ein Pointer für die Struktur
			// im Kernel Pool. Wenn IoCreateDevice scheitert, dann kann es auch NULL sein;

			IoDeleteDevice(DeviceObject);
			return STATUS_FAILED_DRIVER_ENTRY;
		}
		else
		{
			return STATUS_FAILED_DRIVER_ENTRY;
		}
	}

	// Per Default mal alle Handler nicht supporten
	for (int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		DriverObject->MajorFunction[i] = IgnoreIRPHandler;
	}

	// restliche Dispatchfunktionen zuweisen
	DriverObject->MajorFunction[IRP_MJ_CREATE] = IrpCreateHandler;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = IrpCloseHandler;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IrpDeviceIoCtlHandler;


	// Driverunload Funktion zuweisen
	DriverObject->DriverUnload = DriverUnloadHandler;

	// Namen für den DeviceTree erstellen
	// Eigentlich ist diese Art veraltet. IoRegisterDeviceInterface wäre die neure Wahl
	// Über den DosDeviceName kann man den Treiber öffnen, der DeviceName ist eher für interne Zwecke
	IoCreateSymbolicLink(&DosDeviceName, &DeviceName);

	DebugPrint("[KernelControl] Initialisiere Thread Liste\n");
	ThreadLogInit();
	
	// Funktionniert leider nicht richtig
	//DebugPrint("[KernelControl] Der DeviceName ist %s\n", DOSDEVICENAME);
	DebugPrint("[KernelControl] Der Treiber ist nun geladen\n");

	return STATUS_SUCCESS;

}


void DriverUnloadHandler (
	_DRIVER_OBJECT* DriverObject
)
{
	// Hier muss man nichts machen, da kein Pool benutzt wird

	DebugPrintInfo;

	UNREFERENCED_PARAMETER(DriverObject);
	return;
}



NTSTATUS
IrpCreateHandler (
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP Irp
)
{
	// Dies wird aufgerufen, wenn ein neues Handle geöffnet wurde

	DebugPrintInfo;

	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_SUCCESS;

	UNREFERENCED_PARAMETER(DeviceObject);
	PAGED_CODE();
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}



NTSTATUS
IrpCloseHandler (
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP Irp
)
{
	// Dies wird aufgerufen, wenn ein Handle geschlossen wird oder das letzte?
	// Egal: Man kann dies einfach ignorieren

	DebugPrintInfo;

	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_SUCCESS;

	UNREFERENCED_PARAMETER(DeviceObject);
	PAGED_CODE();
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}


NTSTATUS
IgnoreIRPHandler (
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp
)
{
	DebugPrintInfo;

	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;

	UNREFERENCED_PARAMETER(DeviceObject);
	PAGED_CODE();
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_NOT_SUPPORTED;
}



NTSTATUS
IrpDeviceIoCtlHandler(
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP Irp
)
{
	// Dies wird bei Device IOControl aufgerufen

	ULONG					IoControlCode;
	PIO_STACK_LOCATION		IrpSp;
	NTSTATUS				Status = STATUS_SUCCESS;
	PVOID					UserBuffer = NULL;
	long long				address = 0;
	int						ms = 0;

	UNREFERENCED_PARAMETER(DeviceObject);
	PAGED_CODE();

	DebugPrintInfo;

	// Für das Thread Log muss hier noch aufzeichnung kommen
	threadEnter();

	IrpSp = IoGetCurrentIrpStackLocation(Irp);
	IoControlCode = IrpSp->Parameters.DeviceIoControl.IoControlCode;

	// So nun kann man über alles switchen was der Treiber so unterstützt
	switch (IoControlCode)
	{
	case TRIGGER_TRIPLE_FAULT:

		DebugPrint("[KernelControl] ControlCode: TRIGGER_TRIPLE_FAULT_CODE\n");

		TriggerTripleFault();
		Status = STATUS_SUCCESS;
		break;

	case EXECUTE_USERCODE_ADDRESS:

		DebugPrint("[KernelControl] ControlCode: EXECUTE_USERCODE_ADDRESS\n");

		// Zuerst müssen hier nun die Argumente geladen werden
		// Man hat 2 Möglichkeiten: Buffered oder Unbuffered (I/O gibts nicht)

		// Buffered: OS kopiert allen Input von DeviceIoControl in SystemBuffer. Der Treiber kann nun
		// vom SystemBuffer lesen und schreiben, ohne Angst zu haben, dass sich die Daten ändern oder der
		// Treiber eine Access Violation erzeugt. Der SystemBuffer ist dabei Input und Output, also nach
		// der Routine wird der SystemBuffer einfach in den Userland zurückkopiert

		// Unbuffered: Der Treiber liest die Daten direkt vom Userprozess aus. Hier ist mehr Vorsicht geboten,
		// der Treiber 'muss' immer zuerst ProbeForRead und ProbeForWrite auf die Addresse ausführen, sonst kann 
		// es vorkommen, dass er einfach eine falsche Addresse im Kernel überschreibt

		// Unbuffered Verwendet

		// UserBuffer ist nun ein Pointer, welcher zum Buffer im Usermode zeigt
		UserBuffer = IrpSp->Parameters.DeviceIoControl.Type3InputBuffer;

		DebugPrint("Hier ist die Addresse des übergebenen Buffers: %p\n", UserBuffer);

		// ProbeForRead: Checkt, ob UserBuffer im Usermode ist, und eine länge von 8 Bytes hat.
		// Das 1 steht für das Alignment, also die Addresse muss ein Vielfaches vom Alignment sein (1 = egal wo)
		// ProbeForRead hat keinen Rückgabewert sondern erzeugt eine Exception
		__try
		{
			ProbeForRead(UserBuffer, sizeof(PVOID), 1);
		}
		//__except (ExceptCode == STATUS_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			DebugPrint("[KernelControl] Addresse ist nicht lesbar\n");
			// Der Buffer ist nicht lesbar ODER Missalignment (dies wäre ein Bug)
			Status = STATUS_UNSUCCESSFUL;

			// Switch Case beenden, geht das so?
			break;
		}

		// Lade die Addresse aus dem Userbuffer. ACHTUNG: UserBuffer ist nicht das Jump-Target
		address = *(long long*)UserBuffer;

		DebugPrint("[KernelControl] Springe zur Addresse %p\n", address);

		ExecuteUsercodeAddress(address);
		Status = STATUS_SUCCESS;
		break;

	case THREAD_SLEEP:

		DebugPrint("[KernelControl] ControlCode: THREAD_SLEEP\n");
		UserBuffer = IrpSp->Parameters.DeviceIoControl.Type3InputBuffer;

		__try
		{
			ProbeForRead(UserBuffer, sizeof(int), 1);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			break;
		}

		ms = *(int*)UserBuffer;
		ThreadSleep(ms);
		break;

	case CLEAR_SMP:
	{
		DebugPrint("[KernelControl] ControlCode: CLEAR_SMP\n");
		bool result = ClearSMP();

		if (result)
			DebugPrint("[KernelControl] SMEP/SMAP ausgeschaltet\n");
		else
			DebugPrint("[KernelControl] ControlCode: SMEP/SMAP waren bereits deakttivert\n");

		break;
	}

	default:
		Status = STATUS_NOT_SUPPORTED;
		break;
	}

	// Hier ist der Thread dann fertig
	threadLeave();

	return Status;
}
