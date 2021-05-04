#pragma once

#include <wdm.h>






// Debugging Informationen
// Sendet direkt eine Nachricht zu dem Kernel Debugger. Funktionniert Super!

#define DEBUG_PRINTS

#ifdef DEBUG_PRINTS

#define __BANNER__ "[KernelControl] %s"
// Varadisches (mehrere Parameter nach dem Komma) Debug Print
#define DebugPrint(string, ...) DbgPrint(string, ##__VA_ARGS__)
#define DebugPrintInfo DebugPrint("[KernelControl] Funktion %s aufgerufen\n", __func__)


#else

#define DebugPrintInfo
#define DebugPrint() 

#endif







// Konstanten

#define DEVICENAME L"\\Device\\KernelControl"
#define DOSDEVICENAME L"\\DosDevices\\KernelControl"
#define EXT_SIZE 0x100




// Kontollcodes

#define IOCTL(Function) CTL_CODE(FILE_DEVICE_UNKNOWN, Function, METHOD_NEITHER, FILE_ANY_ACCESS)

#define TRIGGER_TRIPLE_FAULT			IOCTL(0x800)
#define EXECUTE_USERCODE_ADDRESS		IOCTL(0x801)
#define THREAD_SLEEP					IOCTL(0x802)
#define CLEAR_SMP						IOCTL(0x803)
#define NOP_FUNC						IOCTL(0x804)













// DriverEntry - Hier startet die Ausführung

DRIVER_INITIALIZE DriverInitialize;

NTSTATUS DriverInitialize(
	_DRIVER_OBJECT* DriverObject,
	PUNICODE_STRING RegistryPath
);


// DriverUnload

DRIVER_UNLOAD DriverUnloadHandler;

void DriverUnDriverUnloadHandler (
	_DRIVER_OBJECT* DriverObject
);



// IRP Funktionen, Driver Dispatching

NTSTATUS
IgnoreIRPHandler(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp
);

NTSTATUS
IrpCreateHandler(
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP Irp
);

NTSTATUS
IrpCloseHandler(
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP Irp
);

NTSTATUS
IrpDeviceIoCtlHandler(
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP Irp
);

