#include <stdio.h>

#include "list.h"
#include "hashtbl.h"

int main ()
{
    struct HashTable_t* hTable = hashT_ctor (1000);
    struct Node_t* node = NULL;

    hTable->buckets[3] = node;

    hashT_dtor (hTable);

    return 0;
}
