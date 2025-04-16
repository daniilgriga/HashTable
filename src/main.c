#include <stdio.h>

#include "list.h"
#include "hashtbl.h"
#include "hash_funcs.h"

const char* filename = "War_and_Peace.txt";
const int HT_SIZE = 2000;

#define FUNC_NAME XXHASH
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

    get_dump (hashT, stdout);
    get_data_for_histo (hashT, FUNC_NAME);

    hashT_dtor (hashT);

    return 0;
}
