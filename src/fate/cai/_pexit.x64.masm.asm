EXTRN fe_cai_pexit:PROC
PUBLIC	_pexit
_TEXT	SEGMENT
_pexit	PROC
    push r11
    push r10
    push r9
    push r8
    push rax
    push rdx
    push rcx
  
    ; reserve space for 4 registers [ RCX,RDX,R8 and R9 ]
    sub rsp,20h
  
    ; Get the return address of the function
    mov rcx,rsp
    mov rcx,qword ptr[rcx+58h]
    sub rcx,5 
    mov rdx,8 ;2nd parameter
    
    ; Call the exported function in dll which finds the name of the function
    call fe_cai_pexit
  
    ; release the reserved space by moving stack pointer up by 32 bytes
    add rsp,20h
    ; pop out the pushed registers
    pop rcx
    pop rdx
    pop rax
    pop r8
    pop r9
    pop r10
    pop r11
    ret
_pexit	ENDP
_TEXT	ENDS
END
