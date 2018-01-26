;encryption and decryption methods for client-side encryption and client/server-side decryption
;@note: Compilation > nasm -f elf64 -o decrypt.o decrypt.asm
;@author: Dandy Martin

;make function visible to linker
global decryption_1
global decryption_2
global decryption_3
global encryption

decryption_1:
            mov rax, rdi                    ;copy first param (char array) into rax
            xor rdi, rdi
            mov rcx, rsi                    ;copy second param (size of char array) into rcx counter
            mov rdx, rsi                    ;copy second param into rdx register
            sub rdx, 3h                     ;subtract 3 from rdx
            mov rbx, rax                    ;copy contents of rax into rbx
            L1:                             ;L# loop for char array
                mov dl, byte [rbx]          ;copy byte of rbx pointer into dl
                sub dl, 1b                  ;subtract bit value from dl
                mov byte [rbx], dl          ;copy bits back into place in rbx
                inc rbx                     ;increment rbx pointer
                loop L1
            mov rcx, rdx
            mov rbx, rax
            add rbx, 3h
            C1:
                xor byte [rbx], 0FFh        ;xor previous byte value after rbx incrementation
                inc rbx
                loop C1                     ;repeat C# until rcx counter == 0
            ret                             ;return back to stack position

decryption_2:
            mov rax, rdi                    ;copy first param (char array) into rax
            xor rdi, rdi
            mov rcx, rsi                    ;copy second param (size of char array) into rcx counter
            mov rdx, rsi                    ;copy second param into rdx register
            sub rdx, 3h                     ;subtract 3 from rdx
            mov rbx, rax                    ;copy contents of rax into rbx
            L2:                             ;L# loop for char array
                mov dl, byte [rbx]          ;copy byte of rbx pointer into dl
                sub dl, 10b                 ;subtract bit value from dl
                mov byte [rbx], dl          ;copy bits back into place in rbx
                inc rbx                     ;increment rbx pointer
                loop L2
            mov rcx, rdx
            mov rbx, rax
            add rbx, 3h
            C2:
                xor byte [rbx], 0FFh        ;xor previous byte value after rbx incrementation
                inc rbx
                loop C2                     ;repeat C# until rcx counter == 0
            ret                             ;return back to stack position

decryption_3:
            mov rax, rdi                    ;copy first param (char array) into rax
            xor rdi, rdi
            mov rcx, rsi                    ;copy second param (size of char array) into rcx counter
            mov rdx, rsi                    ;copy second param into rdx register
            sub rdx, 3h                     ;subtract 3 from rdx
            mov rbx, rax                    ;copy contents of rax into rbx
            L3:                             ;L# loop for char array
                mov dl, byte [rbx]          ;copy byte of rbx pointer into dl
                sub dl, 11b                 ;subtract bit value from dl
                mov byte [rbx], dl          ;copy bits back into place in rbx
                inc rbx                     ;increment rbx pointer
                loop L3
            mov rcx, rdx
            mov rbx, rax
            add rbx, 3h
            C3:
                xor byte [rbx], 0FFh        ;xor previous byte value after rbx incrementation
                inc rbx
                loop C3                     ;repeat C# until rcx counter == 0
            ret                             ;return back to stack position

encryption:
            mov rax, rdi                    ;copy first param (char array) into rax
            mov rcx, rsi                    ;copy second param (size of char array) into rcx counter
            mov rbx, rax                    ;copy contents of rax into rbx
            L4:                             ;L# loop for char array
                mov dl, byte [rbx]          ;copy byte of rbx pointer into dl
                xor dl, 0FFh                ;xor dl value
                mov byte [rbx], dl          ;copy bits back into place in rbx
                inc rbx                     ;increment rbx pointer
                loop L4                     ;repeat L# until rcx counter == 0
            ret                             ;return back to stack position

