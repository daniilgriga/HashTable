#include <stdio.h>

#include "list.h"
#include "hashtbl.h"

const int HT_SIZE = 100;

int main (void)
{
    struct HashTable_t* hashT = hashT_ctor (HT_SIZE);

    hashT_fill (hashT, "text.txt");

    get_dump (hashT, stdout);
    get_data_for_histo (hashT);

    hashT_dtor (hashT);

    return 0;
}
