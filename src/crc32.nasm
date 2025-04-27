
; Compile:  nasm -f elf64 crc32.nasm -o crc32.o

section .text
global hash_crc32_nasm

;|-----REGISTER-----|------VALUE-------|
;|------------------|------------------|
;|       eax        |      hash        |
;|------------------|------------------|
;|       rcx        |    count bytes   |
;|------------------|------------------|

hash_crc32_nasm:
    mov eax, 0xFFFFFFFF
    xor rcx, rcx

.lesgo:
    movzx edx, byte [rdi + rcx]
    test dl, dl
    jz .end

    crc32 eax, dl
    inc rcx
    jmp .lesgo

.end:
    xor eax, 0xFFFFFFFF
    ret

section .note.GNU-stack noalloc noexec nowrite progbits
