
#include <wdm.h>
#include "DriverFunctions.h"


// Definitionen von der Headerdatei

// Dies scheint nicht richtig zu funktionnieren, deshalb wird nun einfach diese
// Funktion in Assembler geschrieben, sodass es sicher funktionniert
/*
void ExecuteUsercodeAddress(long long address)
{
	(void(*)())address;
} 
*/


void ThreadSleep(int ms)
{
	// Kann man Structs so durch einen Default-Konstruktor initioalisieren?
	// Nein sieht nicht so aus. Man muss den Integer selber erstellen und dann einen Pointer übergeben.

	//KeDelayExecutionThread(KernelMode, FALSE, PLARGE_INTEGER(ms));
	// In diesem Falle erstellt man einen Pointer mit dem Wert 'ms'. Der Pointer ist dann logischerweise
	// invalid und es gibt einen crash.
	
	LARGE_INTEGER largeInt;
	largeInt.LowPart = ms;

	KeDelayExecutionThread(KernelMode, FALSE, &largeInt);
}