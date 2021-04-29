
global ExecuteUsercodeAddress
section .text

ExecuteUsercodeAddress:
	
	; Erwarte einen __fastcall
	jmp rcx
