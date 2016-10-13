EXTRN fe_cai_pexit:PROC
PUBLIC	_pexit
_TEXT	SEGMENT
_pexit	PROC
    pushad
    mov  eax, esp
    add  eax, 32
    mov  eax, dword ptr[eax]
    sub  eax, 5
    push eax
    call fe_cai_pexit
    pop eax
    popad
    ret
_pexit	ENDP
_TEXT	ENDS
END
