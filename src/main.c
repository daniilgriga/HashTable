#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "hashtbl.h"
#include "hash_funcs.h"

const char* filename_input = "test/literature/War_and_Peace.bin";
const char* filename_test  = "test/literature/Fahrenhait_451.bin";
const int HT_SIZE = 20000;

#define FUNC_NAME CRC32
// LENGTH
// ASCII
// SUM_POS
// MULT
// DJB2
// SDBM
// CRC32
// FNV_1a
// JENKINS
// XXHASH
// CRC32_INLINE
// CRC32_NASM

int main (void)
{
    struct HashTable_t* hashT = hashT_ctor (HT_SIZE, FUNC_NAME);

    fprintf (stderr, "Start of filling Hash Table\n");
    int num_words_input = hashT_fill (hashT, filename_input);
    if (num_words_input == 0)
        return 1;
    fprintf (stderr, "Hash Table is filled: summary %d words in Hash Table\n", num_words_input);

#if 0
    get_dump (hashT, stdout);
    get_data_for_histo (hashT, FUNC_NAME);
#endif

    int num_words_test = 0;
    char* test_buffer = prepare_TEST (filename_test, &num_words_test);

    fprintf (stderr, "Test is starting: summary %d words for search\n", num_words_test);
    int get_n = hashT_TEST (hashT, test_buffer, num_words_test, 10000);
    fprintf (stderr, "Test is over\n");
    fprintf (stderr, "Number from hashT_TEST = <%d>\n", get_n);

    hashT_dtor (hashT);

    return 0;
}
