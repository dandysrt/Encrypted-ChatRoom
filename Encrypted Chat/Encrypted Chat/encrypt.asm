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
                loop L1                         ;repeat L1 until rcx counter == 0
            ret  8                              ;return back to stack position

encryption_2:
            mov rax, rdi                        ;copy first param (char array) into rax
            mov rcx, rsi                        ;copy second param (size of char array) into rcx counter
            mov rbx, rax                        ;copy contents of rax into rbx
            L2:                                 ;loop through char array
                mov dl, byte [rbx]              ;copy byte of rbx into dl last -> first
                add dl, 10b                      ;add 1bit offset
                mov byte [rbx], dl              ;copy bits back into place in rbx
                inc rbx
                loop L1                         ;repeat L1 until rcx counter == 0
            ret  8                              ;return back to stack position

encryption_3:
            mov rax, rdi                        ;copy first param (char array) into rax
            mov rcx, rsi                        ;copy second param (size of char array) into rcx counter
            mov rbx, rax                        ;copy contents of rax into rbx
            L3:                                 ;loop through char array
                mov dl, byte [rbx]              ;copy byte of rbx into dl last -> first
                add dl, 11b                      ;add 1bit offset
                mov byte [rbx], dl              ;copy bits back into place in rbx
                inc rbx
                loop L1                         ;repeat L1 until rcx counter == 0
            ret  8                              ;return back to stack position
