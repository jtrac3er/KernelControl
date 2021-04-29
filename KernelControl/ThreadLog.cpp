
#include "ThreadLog.h"




void threadEnter()
{
	ExAcquireFastMutex(&tLogMutex);			// mit dem Mutex die Liste locken

	const THREAD thread = getCurrentThreadID();

	for (int i = 0; i < LEN(tLog.threadList); i++)
	{
		if (!tLog.threadList[i])
		{
			// Wiese einen neuen TID zu
			tLog.threadList[i] = thread;
			goto success;
		}
	}

//failure:
	DebugPrint("[KernelControl] <ThreadLog>: Konnte keinen neuen Platz finden. ");
	DebugPrint("Die Variablen sind: tLog.counter = %i\n", tLog.counter);

success:
	tLog.counter--;							// Inkrement
	ExReleaseFastMutex(&tLogMutex);
	return;
}




void threadLeave()
{
	ExAcquireFastMutex(&tLogMutex);		// mit dem Mutex die Liste locken
	
	const THREAD thread = getCurrentThreadID();

	for (int i = 0; i < LEN(tLog.threadList); i++)
	{
		if (tLog.threadList[i] == thread)
		{
			// Wiese einen neuen TID zu
			tLog.threadList[i] = 0;
			goto success;
		}
	}

//failure:
	DebugPrint("[KernelControl] <ThreadLog>: Konnte Eintrag nicht löschen. ");
	DebugPrint("Die Variablen sind: thread: %ll, tLog.counter = %i\n", thread, tLog.counter);

success:
	tLog.counter++;						// Dekrement
	ExReleaseFastMutex(&tLogMutex);
	return;
}




void ThreadLogInit()
{
	ExInitializeFastMutex(&tLogMutex);	// Fast Mutex initialisieren
}