KernelControl - KMode Treiber, um mit dem Kernel zu interagieren und verschiedene Szenarien zu testen. 

Der Treiber soll einfache IOCTL-Operationen verwenden, um gewisse Sachen im Kernel zu testen. Er wird gebraucht, um ein besseres Verständnis zum Windows Kernel zu bekommen.
Erweiterbarkeit steht im Vordergrund. Es ist ganz einfach, neue Operationen hinzuzufügen, weil alle Operationen eigentlich mehr oder weniger Unabhängig voneinander sind. 

Wofür wird der Treiber entwickelt?
  * Um Sachen zu testen. Dies ist vorallem meiner Hacking-Reihe "Windows NT: Privilege Escalation" dienen. Der Link hierzu findet man hier:
    https://hackblog.internet-box.ch/

Was kann der Treiber schon?
  * Vom Kernel aus zu Usercode springen
  * Bestimme Control Bits der CPU abändern
  * Einen sofortigen Triple-Fault erzeugen
  
Was soll der Treiber noch können?
  * Erweiterte Funktionalitäten wie IDT/SSDT Hooking
  * Read/Write für den Kernel Mode (Debug)
