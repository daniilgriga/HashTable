#include <stdio.h>

#include "list.h"
#include "hashtbl.h"

int main (void)
{
    struct HashTable_t* hashT = hashT_ctor (10);

    hashT_fill (hashT, "text.txt");

    hashT_dtor (hashT);

    return 0;
}

//! for graphiz >>> './build/test | dot -Tpng -o linked_list.png'
