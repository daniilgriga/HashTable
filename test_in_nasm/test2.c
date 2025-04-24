
// Compile and link:  gcc -msse4.2 test2.c crc32.o -o test_crc32

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "crc32_nasm.h"

static inline uint32_t hash_CRC32_inline (const char* key)
{
    uint32_t hash = 0xFFFFFFFF;

    while (*key)
    {
        uint8_t byte = (uint8_t)(*key++);
        __asm__ volatile ("crc32b %1, %0" : "+r"(hash) : "r"(byte));
    }
    return hash ^ 0xFFFFFFFF;
}

int main (void)
{
    const char* test_strings[] = {"", "a", "hello", "world", "1234567890", NULL};

    for (int i = 0; test_strings[i]; i++)
    {
        const char* str = test_strings[i];
        uint32_t inline_hash = hash_CRC32_inline (str);
        uint32_t asm_hash = hash_crc32 (str);

        fprintf (stderr, "String: '%s'\n", str);
        fprintf (stderr, "Inline hash: 0x%08X\n", inline_hash);
        fprintf (stderr, "NASM hash:   0x%08X\n", asm_hash);
        fprintf (stderr, "Hashes: %s\n\n", inline_hash == asm_hash ? "match" : "differ");
    }

    return 0;
}
