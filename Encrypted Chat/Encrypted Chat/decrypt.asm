;make function visible to linker
global decryption_1
global decryption_2
global decryption_3
global encryption

decryption_1:
            mov rax, rdi        ;copy first param (char array) into rax
            mov rcx, rsi        ;copy second param (size of char array) into rcx counter
            mov rbx, rax        ;copy contents of rax into rbx
            L1:                 ;loop through char array
                mov dl, byte [rbx]  ;copy byte of rbx into diladd dl, 1b
                sub dl, 1b
                mov byte [rbx], dl  ;copy bits back into place in rbx
                xor byte [rbx], 0FFh
                inc rbx
                loop L1         ;repeat L1 until rcx counter == 0
            ret                 ;return back to stack position

decryption_2:
            mov rax, rdi        ;copy first param (char array) into rax
            mov rcx, rsi        ;copy second param (size of char array) into rcx counter
            mov rbx, rax        ;copy contents of rax into rbx
            L2:                 ;loop through char array
                mov dl, byte [rbx]  ;copy byte of rbx into diladd dl, 1b
                sub dl, 10b
                mov byte [rbx], dl  ;copy bits back into place in rbx
                xor byte [rbx], 0FFh
                inc rbx
                loop L2         ;repeat L1 until rcx counter == 0
            ret                 ;return back to stack position

decryption_3:
            mov rax, rdi        ;copy first param (char array) into rax
            mov rcx, rsi        ;copy second param (size of char array) into rcx counter
            mov rbx, rax        ;copy contents of rax into rbx
            L3:                 ;loop through char array
                mov dl, byte [rbx]  ;copy byte of rbx into diladd dl, 1b
                sub dl, 11b
                mov byte [rbx], dl  ;copy bits back into place in rbx
                xor byte [rbx], 0FFh
                inc rbx
                loop L3         ;repeat L1 until rcx counter == 0
            ret                 ;return back to stack position

encryption:
            mov rax, rdi
            mov rcx, rsi
            mov rbx, rax
            L4:
                xor byte [rbx], 0FFh
                inc rbx
                loop L4
            ret

