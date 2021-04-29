
BITS 64
global ClearSMP
section .text

; Die Bitmask ist auf die jeweiligen Bits gecleart, also 
; Die Bits für SMEP/SMAP sind 0, der Rest 1

; 20	SMEP	Supervisor Mode Executions Protection Enable
; 21	SMAP	Supervisor Mode Access Protection Enable

BITMASK equ ~((0x1 << 20)  | (0x1 << 21))

ClearSMP:

	mov rax, cr4
	mov rbx, rax		; rbx volatil? Glaub schon
	and rax, BITMASK	; CLeare die entsprechenden Bits aus dem aktuellen CR4 Wert
	cmp rax, rbx		; Schaue, ob dies einen Unterschied zu vorher war
	je SMP_ALREADY_DISABLED
	mov cr4, rax		; Neuen Wert schreiben
	xor rax, rax		; NASM spinnt hier glaub: Es nimmt untern EAX statt RAX
	mov rax, 1			; Setzte Return = TRUE = 1
	ret

	SMP_ALREADY_DISABLED:
		
		; In diesem Falle waren beide schon deaktiviert
		; Nichts hat sich durch das clearen verändert
		; Returne den Boolean Wert FALSE = 0
		xor rax, rax
		ret
