
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
	KeDelayExecutionThread(KernelMode, FALSE, PLARGE_INTEGER(ms));
}