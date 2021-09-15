#pragma once

#include <wdm.h>

#define MAX_FILENAME_LENGTH 1024

typedef PID long long;
typedef ADDRESS long long;
typedef REGISTER long long;

// globals

bool isMonitoring;		// ist der Treiber gerade am aufzeichnen?
bool isSetUp;			// sind die notwendigen Informationen zum monitoring gegeben?
bool hasLogFile;		// Ist ein logfile Handle vorhanden?

PID currentMonitoringProcess;			// welcher Prozess wird aufgezeichnet
ADDRESS currentFinishedJumpTarget;		// wohin muss gesprungen werden?
wchar_t* currentLogfileName;				// name der aktuellen Logfile

FAST_MUTEX mutex;		// Es sollte noch einen globalen Mutex geben
HANDLE logFileHandle;	// Dies ist die Datei, wo geschrieben wird

struct MONITORING_INFORMATION
{
	PID monitoringPid;
	wchar_t logfileName[MAX_FILENAME_LENGTH];
};

struct HOOKING_INFORMATION
{
	ADDRESS finishedJumpTarget;
};

struct LOG
{
	REGISTER Rax;
	TIME timestamp;
	long long number;
};

// Mit dieser Funktion kann man den Treiber aufsetzen
bool setHookingInformation(HOOKING_INFORMATION* info);

// Mit dieser Funktion kann man das Monitoring einrichten
bool setMonitoringInformation(MONITORING_INFORMATION* info);

// Hier kann man das Monitoring starten/beenden
bool setMonitoringState(bool monitoring);

// Dies ist das eigentliche Callback, wird die direkt aufgerufen
void syscallHookCb();

// Diese Methode wird vom Assembler-Hookcode aufgerufen
// Es enthält zunächst mal nur das RAX-Register
void putInformation(REGISTER rax);

void syscallHookInit();