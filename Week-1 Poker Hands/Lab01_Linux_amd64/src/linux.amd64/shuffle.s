.intel_syntax noprefix

.global shuffle

.text
shuffle:
        push rbx
        push r12
        mov r12, rdi  
        mov ecx, 51   

.loop:
        push rcx
        call rand@PLT
        pop rcx

        xor edx, edx
        lea ebx, [rcx + 1]  
        div ebx             

        mov eax, [r12 + rcx*4]
        mov ebx, [r12 + rdx*4]
        mov [r12 + rdx*4], eax
        mov [r12 + rcx*4], ebx

        dec ecx
        jns .loop  

        pop r12
        pop rbx
        ret
