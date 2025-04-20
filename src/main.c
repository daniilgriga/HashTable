#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "hashtbl.h"
#include "hash_funcs.h"

const char* filename = "test/War_and_Peace.txt";
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

int main (void)
{
    struct HashTable_t* hashT = hashT_ctor (HT_SIZE, FUNC_NAME);


    int num_words = 0;
    char** words_arr = hashT_fill (hashT, filename, &num_words);

#if 0
    get_dump (hashT, stdout);
    get_data_for_histo (hashT, FUNC_NAME);
#endif

    hashT_TEST (hashT, words_arr, num_words, 2000);

    for (int i = 0; i < num_words; i++)
        free(words_arr[i]);
    free (words_arr);

    hashT_dtor (hashT);

    return 0;
}
