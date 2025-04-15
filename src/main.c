#include <stdio.h>

#include "list.h"
#include "hashtbl.h"

const char* filename = "Fahrenhait_451.txt";
const int HT_SIZE = 1000;

#define FUNC_NAME ASCII

int main (void)
{
    struct HashTable_t* hashT = hashT_ctor (HT_SIZE, FUNC_NAME);

    hashT_fill (hashT, filename);

    get_dump (hashT, stdout);
    get_data_for_histo (hashT, FUNC_NAME);

    hashT_dtor (hashT);

    return 0;
}
