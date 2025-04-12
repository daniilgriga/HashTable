#ifndef HASHTBL
#define HASHTBL

struct HashTable_t
{
    struct Node_t** buckets;
    size_t size;
};

struct HashTable_t* hashT_ctor (size_t size);

#endif
