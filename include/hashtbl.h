#ifndef HASHTBL
#define HASHTBL

#include <stdint.h>

struct HashTable_t
{
    struct Node_t** buckets;
    long size;
};

uint32_t DJB2_hash (const char* str);

struct HashTable_t* hashT_ctor (long size);

int hashT_dtor (struct HashTable_t* hsh_t);

int hashT_fill (struct HashTable_t* hashT_ptr, const char* filename);

int hashT_search (struct HashTable_t* hashT_ptr, const char* data, uint32_t* hash);

int hashT_insert (struct HashTable_t* hashT_ptr, const char* data, uint32_t* hash);

#endif
