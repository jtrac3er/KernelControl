#pragma once

#include <wdm.h>
#include "DriverEntry.h"

/*

Diese Datei enthält Informationen zu den Aktuell im Treiber laufenden Threads

Wichtig: Dies betrifft nur Threads, welche über ein IOCTL gekommen sind. Andere
Threads werden nicht geloggt

*/



#define MAX_THREADS 256
#define THREAD long long
#define LEN(arr) sizeof(arr) / sizeof(arr[0])








// Ein bisschen unschön aber sonst werden die structs und globale Variablen 
// doppelt definiert, da diese Datei auch in DriverEntry.cpp verwendet wird.

#ifndef FUNC_ONLY


struct ThreadLog
{
	int counter;
	THREAD threadList[MAX_THREADS];
};

ThreadLog tLog;				// tLog wird hier in .data global erstellt

FAST_MUTEX tLogMutex;		// Es sollte noch einen globalen Mutex geben


#endif







void threadEnter();		// Aufrufen, wenn Thread reinkommt

void threadLeave();		// Aufrufen, wenn Thread geht

void ThreadLogInit();	// Mutex erstellen

extern "C" THREAD getCurrentThreadID();		// In Assembler