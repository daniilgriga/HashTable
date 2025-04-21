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

char** hashT_fill (struct HashTable_t* hashT_ptr, const char* filename, int* num_words);

int hashT_fill_search (struct HashTable_t* hashT_ptr, const char* data, uint32_t* hash);

int hashT_search (struct HashTable_t* hashT_ptr, const char* data);

int hashT_insert (struct HashTable_t* hashT_ptr, const char* data, uint32_t* hash);

int get_dump (struct HashTable_t* hashT_ptr, FILE* file);

void get_data_for_histo (struct HashTable_t* hashT_ptr, enum Functions name);

int hashT_TEST (struct HashTable_t* hashT_ptr, char** words_arr, int num_words, size_t num_tests);

#endif // HASHTBL_H
