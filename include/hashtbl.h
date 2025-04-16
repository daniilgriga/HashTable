#ifndef HASHTBL_H
#define HASHTBL_H

#include <stdint.h>

#include "hash_funcs.h"

typedef unsigned int (*HashFunction)(const char*);

struct HashTable_t
{
    struct Node_t** buckets;
    long size;
    HashFunction func_ptr;
};

HashFunction select_function (enum Functions name);

struct HashTable_t* hashT_ctor (long size, enum Functions name);

int hashT_dtor (struct HashTable_t* hsh_t);

int hashT_fill (struct HashTable_t* hashT_ptr, const char* filename);

int hashT_search (struct HashTable_t* hashT_ptr, const char* data, uint32_t* hash);

int hashT_insert (struct HashTable_t* hashT_ptr, const char* data, uint32_t* hash);

int get_dump (struct HashTable_t* hashT_ptr, FILE* file);

void get_data_for_histo (struct HashTable_t* hashT_ptr, enum Functions name);

#endif // HASHTBL_H
