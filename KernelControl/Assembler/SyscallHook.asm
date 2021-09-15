BITS 64

; Exportiere die Symbole
global syscallHookCb

; importierere andere symbole
extern putInformation
extern currentFinishedJumpTarget


section .text

syscallHookCb:

	; zuerst m�ssen mal alle Register gelesen werden
	push rax
	
	; rufe eine Treiberfunktion auf. Diese verwendet stdcall, um
	; das RAX-Register zu erhalten
	call putInformation

	; Epilog: Hier m�ssen andere Instruktionen ersetzt werden
	push r11
	push 0x33
	push rcx
	
	; Nun muss wieder zur�ckgekehrt werden
	jmp [currentFinishedJumpTarget]
	