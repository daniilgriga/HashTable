#include <stdio.h>

#include "list.h"
#include "hashtbl.h"

int main (void)
{
    struct HashTable_t* hashT = hashT_ctor (10);

    hashT_fill (hashT, "text.txt");

    get_dump (hashT, stdout);

    hashT_dtor (hashT);

    return 0;
}

// FIXME - words in HT with pucntuation signs
