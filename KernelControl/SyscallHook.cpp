
// Hier sollen alle Hooking Funktionen hineingeschrieben werden

#include <wdm.h>
#include "SyscallHook.h"
#include "DriverEntry.h"


// Mit dieser Funktion kann man den Treiber aufsetzen
NTSTATUS setHookingInformation(HOOKING_INFORMATION* info)
{
	currentFinishedJumpTarget = info->finishedJumpTarget;
	return STATUS_SUCCESS;
}

// Mit dieser Funktion kann man das Monitoring einrichten
NTSTATUS setMonitoringInformation(MONITORING_INFORMATION* info)
{
	currentMonitoringProcess = info->monitoringPid;
	currentLogfileName = info->logfileName;

	// Logfile öffnen
	NTSTATUS			ntstatus;
	IO_STATUS_BLOCK		ioStatusBlock;
	UNICODE_STRING		uniName;
	OBJECT_ATTRIBUTES	objAttr;

	if (hasLogFile)
	{
		// Es gibt bereits ein logfile Handle, welches geschlossen werden muss
		hasLogFile = false;
		ZwClose(logFileHandle);
	}

	// Wandle den String in ein Unicode um
	RtlInitUnicodeString(&uniName, currentLogfileName);

	// Boilerplate
	InitializeObjectAttributes(
		&objAttr, &uniName,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL, NULL
	);

	// Boilerplate
	ntstatus = ZwCreateFile(&logFileHandle,
		GENERIC_WRITE,
		&objAttr, &ioStatusBlock, NULL,
		FILE_ATTRIBUTE_NORMAL,
		0,
		FILE_OVERWRITE_IF,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL, 0
	);

	if (!NT_SUCCESS(ntstatus))
	{
		// etwas ist schiefgelaufen
		DebugPrint("Konnte kein Handle erlangen. Status ist %x", ntstatus);
		return ntstatus;
	}
	else
	{
		hasLogFile = true;
		return STATUS_SUCCESS;
	}
}

// Hier kann man das Monitoring starten/beenden
NTSTATUS setMonitoringState(bool monitoring)
{
	if (monitoring == isMonitoring)
		return false;
	isMonitoring = monitoring;
	return STATUS_SUCCESS;
}

void putInformation(REGISTER rax)
{
	if (!hasLogFile) return;
	if (!isMonitoring) return;
	if (!isSetUp) return;
	
	PEPROCESS currentProcess = PsGetCurrentProcess();

	if (currentProcess->UniqueProcessId != currentMonitoringProcess) return;

	// In diesem Falle ist alles OK um aufgezeichnet zu werden
	LOG log = { rax, __rdtsc(), 0 };	// erstelle die Daten
	LARGE_INTEGER offset = { -1 };		// -1, um zu appenden

	NTSTATUS status = ZwWriteFile(
		logFileHandle,
		NULL, NULL, NULL, NULL,
		&log, sizeof(log), &offset,
		NULL
	);

	if (!NT_SUCCESS(status))
	{
		DebugPrint("Konnte nich in die Logdatei schreiben. Status = %x", status);
	}

}
