#pragma once

/*

Hier kommen die eigenen Funktionen hin, �bersichtlicher

*/


// Diese Funktion triggert einen TripleFault indem das CR3-Register manipuliert wird
// Es wird in Assembler geschrieben und dann gelinkt

extern "C" void TriggerTripleFault (void);

// Diese Funktion spring zu Usercode, dessen Addresse als Argument �bergeben wurde
// Dabei werden keine Checks gemacht, sondern einfach gejumpt
// Die Funktion wird einfach hier in dieser Headerdatei definiert

extern "C" void __fastcall ExecuteUsercodeAddress(long long address);


// Diese Funktion pausiert einfach einen Thread im Kernel f�r eine bestimmte Zeit
// Also eigenlich TimeSleep einfach f�r einen Treiber

void ThreadSleep(int ms);


// Diese Funktion soll Infos �ber die Anzahl Threads ausgeben

void getThreadLogInfo();


// Diese Funktion soll noch SMEP/SMAP deaktivieren

extern "C" bool ClearSMP();