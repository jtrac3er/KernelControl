
BITS 64

; Exportiere die Symbole
global TriggerTripleFault


; Hier muss man unbdedingt die Sektion angeben, sonst werden symbole nicht erkannt
section .text

TriggerTripleFault:

	xor rax, rax
	mov cr3, rax		; CR3 ist f�r Paging, so wird DIREKT ein TripleFault erzeugt
						; Bei CR4 wird lediglich eine Exception geneierert, welche erst
						; sp�ter vielleicht in einem Double/Triple-Fault endet
	ret

	
; Kleine Unsch�nheit: Hier werden beide Funktionen immer zusammengeschweisst.
; Der Linker kann nat�rlich nicht erkennen, dass die beiden Funktionen unabh�ngig 
; voneinander sind. Irgendwie m�sste man dies bei NASM einstellen oder einfach
; f�r jede Funktion eine eigene Datei erstellen. Es funktionniert aber auch so,
; sieht einfach nicht so gut aus und macht das Debuggen schwerer, da keine INT3
; Padding Barriere zwischen den Funktionen existiert

; Deshalb: Verschiebe in eigene Datei. Um besser alle generierten Object-Dateien einzuschliessen
; m�ssen Wildcards im Projektmanager verwendet werden.
