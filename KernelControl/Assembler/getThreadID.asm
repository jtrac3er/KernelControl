
; Da es so aussieht, als ob es keine Funktion gibt, die TID auszugeben, wird es nun hier manuell gemacht
; Man kann verwenden, dass die TID im TEB steht, und diese in assembly addressieren ist einfach

; TEB: https://en.wikipedia.org/wiki/Win32_Thread_Information_Block
; pointer	FS:[0x24]	GS:[0x48]	NT	Current thread ID

BITS 64

global getCurrentThreadID
section .text

getCurrentThreadID: 
	
	mov rax, [gs:0x48]
	ret

