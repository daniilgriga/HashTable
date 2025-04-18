#include <stdio.h>

#include "list.h"
#include "hashtbl.h"
#include "hash_funcs.h"

const char* filename = "test/War_and_Peace.txt";
const int HT_SIZE = 2000;

#define FUNC_NAME JENKINS
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

    hashT_fill (hashT, filename);

#if 0
    get_dump (hashT, stdout);
    get_data_for_histo (hashT, FUNC_NAME);
#endif

    hashT_dtor (hashT);

    return 0;
}
