;encryption methods for server-side encryption (bit addition)
;@note : Compilation > nasm -f elf64 -o encrypt.o encrypt.asm
;@author : Dandy Martin

;make functions visible to linker
global encryption_1
global encryption_2
global encryption_3

encryption_1:
            mov rax, rdi                        ;copy first param (char array) into rax
            mov rcx, rsi                        ;copy second param (size of char array) into rcx counter
            mov rbx, rax                        ;copy contents of rax into rbx
            L1:                                 ;loop through char array
                mov dl, byte [rbx]              ;copy byte of rbx into dl last -> first
                add dl, 1b                      ;add 1bit offset
                mov byte [rbx], dl              ;copy bits back into place in rbx
                inc rbx
                loop L1                         ;repeat L# until rcx counter == 0
            ret                                 ;return back to stack position

encryption_2:
            mov rax, rdi                        ;copy first param (char array) into rax
            mov rcx, rsi                        ;copy second param (size of char array) into rcx counter
            mov rbx, rax                        ;copy contents of rax into rbx
            L2:                                 ;loop through char array
                mov dl, byte [rbx]              ;copy byte of rbx into dl last -> first
                add dl, 10b                     ;add 2bit offset
                mov byte [rbx], dl              ;copy bits back into place in rbx
                inc rbx
                loop L2                         ;repeat L# until rcx counter == 0
            ret                                 ;return back to stack position

encryption_3:
            mov rax, rdi                        ;copy first param (char array) into rax
            mov rcx, rsi                        ;copy second param (size of char array) into rcx counter
            mov rbx, rax                        ;copy contents of rax into rbx
            L3:                                 ;loop through char array
                mov dl, byte [rbx]              ;copy byte of rbx into dl last -> first
                add dl, 11b                     ;add 3bit offset
                mov byte [rbx], dl              ;copy bits back into place in rbx
                inc rbx
                loop L3                         ;repeat L# until rcx counter == 0
            ret                                 ;return back to stack position
