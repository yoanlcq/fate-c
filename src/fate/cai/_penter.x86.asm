EXTRN fe_cai_penter:PROC
PUBLIC	_penter
_TEXT	SEGMENT
_penter	PROC
    pushad
    mov  eax, esp
    add  eax, 32
    mov  eax, dword ptr[eax]
    sub  eax, 5
    push eax
    call fe_cai_penter
    pop eax
    popad
    ret
_penter	ENDP
_TEXT	ENDS
END
