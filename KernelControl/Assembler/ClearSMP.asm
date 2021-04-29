
BITS 64
global ClearSMP
section .text

; Die Bitmask ist auf die jeweiligen Bits gecleart, also 
; Die Bits für SMEP/SMAP sind 0, der Rest 1
BITMASK equ 0x11

ClearSMP:

	mov rax, cr4
	mov rbx, rax		; rbx volatil? Glaub schon
	and rax, BITMASK
	cmp rax, rbx		
	je SMP_ALREADY_DISABLED

	mov cr4, rax		; Neuen Wert schreiben
	mov rax, 1			; Setzte Return = TRUE = 1
	ret

	SMP_ALREADY_DISABLED:
		
		; In diesem Falle waren beide schon deaktiviert
		; Nichts hat sich durch das clearen verändert
		; Returne den Boolean Wert FALSE = 0

		xor rax, rax
		ret
