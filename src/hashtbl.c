//#define RESEARCH

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "hashtbl.h"
#include "hash_funcs.h"
#include "crc32_nasm.h"
#include "file.h"
#include "list.h"
#include "tools.h"

#define HASH_CRC32_(text) hash_CRC32_inline (text);

HashFunction select_function (enum Functions name)
{
    switch (name)
    {
        case LENGTH:       fprintf (stderr, "LENGTH\n");      return hash_LENGTH;
        case ASCII:        fprintf (stderr, "ASCII\n");       return hash_ASCII;
        case SUM_POS:      fprintf (stderr, "SUM_POS\n");     return hash_SUM_POS;
        case MULT:         fprintf (stderr, "MULT\n");        return hash_MULT;
        case DJB2:         fprintf (stderr, "DJB2\n");        return hash_DJB2;
        case SDBM:         fprintf (stderr, "SDBM\n");        return hash_SDBM;
        case CRC32:        fprintf (stderr, "CRC32\n");       return hash_CRC32;
        case FNV_1a:       fprintf (stderr, "FNV_1a\n");      return hash_FNV_1a;
        case JENKINS:      fprintf (stderr, "JENKINS\n");     return hash_JENKINS;
        case XXHASH:       fprintf (stderr, "XXHASH\n");      return hash_XXHASH;
        case CRC32_INLINE: fprintf (stderr, "CRC32INLINE\n"); return hash_CRC32_inline;
        case CRC32_NASM:   fprintf (stderr, "CRC32NASM\n");   return hash_CRC32_nasm;

        default:
            fprintf (stderr, "There is no that hash function\n");
            return NULL;
    }
}

struct HashTable_t* hashT_ctor (long size, enum Functions name)
{
    struct HashTable_t* hsh_t = (struct HashTable_t*) calloc (1, sizeof (struct HashTable_t));
    if (hsh_t == NULL)
    {
        ERROR_MESSAGE (NULL_PTR_ERR)
        return NULL;
    }

    // ? maybe one calloc -> buckets starts after struct Hashtable_t* ? //

    hsh_t->buckets = (struct Node_t**) calloc ((size_t) size, sizeof (struct Node_t*));
    if (hsh_t->buckets == NULL)
    {
        ERROR_MESSAGE (NULL_PTR_ERR)
        return NULL;
    }

    hsh_t->size = size;
    hsh_t->func_ptr = select_function (name);

    return hsh_t;
}

int hashT_dtor (struct HashTable_t* hashT_ptr)
{
    assert (hashT_ptr);

    for (long i = 0; i < hashT_ptr->size; i++)
    {
        if (hashT_ptr->buckets[i] != NULL)
            list_dtor (hashT_ptr->buckets[i]);
    }

    free (hashT_ptr->buckets);
    free (hashT_ptr);

    return NO_ERRORS;
}

int hashT_fill (struct HashTable_t* hashT_ptr, const char* filename)
{
    assert (hashT_ptr);
    assert (filename);

    long num_symb = 0;
    char* buffer = MakeBuffer (filename, &num_symb);
    if (buffer == NULL)
        return 0;

    int num_words = 0;
    char* ptr = buffer;
    while (*ptr)
    {
        char* word = ptr;
        while (*ptr != '\0')
            ptr++;

        int length = (int) (ptr - word);

        uint32_t hash = 0;
        int status = hashT_fill_search (hashT_ptr, word, &hash);
        if (status)
        {
            hashT_insert (hashT_ptr, word, &hash);
            num_words++;
        }

        ptr += MAX_LENGTH_WORD - length;
    }

    free (buffer);

    return num_words;
}

int hashT_fill_search (struct HashTable_t* hashT_ptr, const char* data, uint32_t* hash)
{
    assert (hashT_ptr);
    assert (data);

#ifdef RESEARCH
    *hash = hashT_ptr->func_ptr (data);
#else
    *hash = HASH_CRC32_(data)
#endif

    *hash %= (uint32_t) hashT_ptr->size;

    struct Node_t* node = hashT_ptr->buckets[*hash];
    if (node == NULL)
        return 1;

    struct Node_t* found_node = list_search (node, data);
    if (found_node == NULL)
        return 1;

    return 0;
}

int hashT_insert (struct HashTable_t* hashT_ptr, const char* data, uint32_t* hash)
{
    assert (hashT_ptr);
    assert (data);
    assert (hash);

    struct Node_t* curr_bucket = hashT_ptr->buckets[*hash];
    if (curr_bucket == NULL)
    {
        hashT_ptr->buckets[*hash] = node_insert (curr_bucket, data);
        return 0;
    }

    //fprintf (stderr, "\t\t\t curr_bucket = [%p]\n", curr_bucket);
    while (curr_bucket->next != NULL)
        curr_bucket = curr_bucket->next;
    //fprintf (stderr, "\t\t\t curr_bucket = [%p] after while\n", curr_bucket);

    curr_bucket = node_insert (curr_bucket, data);

    return 0;
}

int get_dump (struct HashTable_t* hashT_ptr, FILE* file)
{
    assert (hashT_ptr);
    assert (file);

    fprintf (file, "digraph HashTable {\n");
    fprintf (file, "    node [shape=record];\n");
    fprintf (file, "    rankdir=HT;\n");
    fprintf (file, "    splines=false;\n");
    fprintf (file, "    hashtable [style=filled, fillcolor=lightblue, height=0.8, width=%ld.0 label=\"", hashT_ptr->size);

    for (int i = 0; i < hashT_ptr->size; i++)
    {
        fprintf (file, "<f%d> [%d]", i, i);
        if (i < hashT_ptr->size - 1)
            fprintf (file, "|");
    }

    fprintf (file, "\", color=purple];\n");

    for (long i = 0; i < hashT_ptr->size; i++)
    {
        struct Node_t* curr = hashT_ptr->buckets[i];
        long lists_cntr = 0;

        if (!curr) continue;

        while (curr)
        {
            fprintf (file, "    node%ld_%ld [style=filled, fillcolor=pink, label=\"%s\", color=blue];\n", i, lists_cntr, curr->data);

            if (lists_cntr > 0)
                fprintf (file, "    node%ld_%ld -> node%ld_%ld;\n", i, lists_cntr-1, i, lists_cntr);

            curr = curr->next;
            lists_cntr++;
        }

        if (hashT_ptr->buckets[i] && hashT_ptr->buckets[i]->data)
            fprintf (file, "    hashtable:f%ld -> node%ld_0;\n", i, i);
    }

    fprintf (file, "}\n");

    return 0;
}

void get_data_for_histo (struct HashTable_t* hashT_ptr, enum Functions name)
{
    FILE* file = OpenFile ("test/data.txt", "wb");
    assert (file);

    switch (name)
    {
        case LENGTH:       fprintf (file, "LENGTH\n");       break;
        case ASCII:        fprintf (file, "ASCII\n");        break;
        case SUM_POS:      fprintf (file, "SUM_POS\n");      break;
        case MULT:         fprintf (file, "MULT\n");         break;
        case DJB2:         fprintf (file, "DJB2\n");         break;
        case SDBM:         fprintf (file, "SDBM\n");         break;
        case CRC32:        fprintf (file, "CRC32\n");        break;
        case FNV_1a:       fprintf (file, "FNV_1a\n");       break;
        case JENKINS:      fprintf (file, "JENKINS\n");      break;
        case XXHASH:       fprintf (file, "XXHASH\n");       break;
        case CRC32_INLINE: fprintf (file, "CRC32_INLINE\n"); break;
        case CRC32_NASM:   fprintf (file, "CRC32NASM\n");    break;

        default:
            fprintf (file, "I dont know wtf is this function\n");
    }

    for (long i = 0; i < hashT_ptr->size; i++)
    {
        fprintf (file, "%ld,", i);

        int j = 0;
        struct Node_t* ptr = hashT_ptr->buckets[i];

        while (ptr)
        {
            j++;
            ptr = ptr->next;
        }

        fprintf (file, "%d\n", j);
    }

    CloseFile (file);
}

int hashT_search (struct HashTable_t* hashT_ptr, const char* data)
{
    assert (hashT_ptr);
    assert (data);

#ifdef RESEARCH
    uint32_t hash = hashT_ptr->func_ptr (data);
#else
    uint32_t hash = HASH_CRC32_(data)
#endif

    hash %= (uint32_t) hashT_ptr->size;

    if (hashT_ptr->buckets[hash] == NULL)
    {
        return 0;
    }
    else
    {
        struct Node_t* node = list_search (hashT_ptr->buckets[hash], data);
        if (node == NULL)
            return 0;
        else
            return 1;
    }
}

char* prepare_TEST (const char* filename, int* num_words)
{
    assert (filename);

    long num_symb = 0;
    char* buffer = MakeBuffer (filename, &num_symb);
    if (buffer == NULL)
        return 0;

    int counter_words = 0;
    char* ptr = buffer;
    while (*ptr)
    {
        char* word = ptr;
        while (*ptr != '\0')
            ptr++;

        counter_words++;

        int length = (int) (ptr - word);
        ptr += MAX_LENGTH_WORD - length;
    }

    *num_words = counter_words;

    return buffer;
}

int hashT_TEST (struct HashTable_t* hashT_ptr, char* test_buffer, int num_words, size_t num_tests)
{
    int get = 0;

    char* old_ptr = test_buffer;

    for (size_t i = 0; i < num_tests; i++)
    {
        test_buffer = old_ptr;

        for (int word_i = 0; word_i < num_words; word_i++)
        {
            get += hashT_search (hashT_ptr, test_buffer);
            test_buffer += 32;
        }
    }
    return get;
}
