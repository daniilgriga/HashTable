#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <immintrin.h>

#include "hash_funcs.h"
#include "hashtbl.h"
#include "file.h"
#include "list.h"
#include "tools.h"

HashFunction select_function (enum Functions name)
{
    switch (name)
    {
        case LENGTH:       fprintf (stderr, "LENGTH\n");    return hash_LENGTH;
        case ASCII:        fprintf (stderr, "ASCII\n");     return hash_ASCII;
        case SUM_POS:      fprintf (stderr, "SUM_POS\n");   return hash_SUM_POS;
        case MULT:         fprintf (stderr, "MULT\n");      return hash_MULT;
        case DJB2:         fprintf (stderr, "DJB2\n");      return hash_DJB2;
        case SDBM:         fprintf (stderr, "SDBM\n");      return hash_SDBM;
        case CRC32:        fprintf (stderr, "CRC32\n");     return hash_CRC32;
        case FNV_1a:       fprintf (stderr, "FNV_1a\n");    return hash_FNV_1a;
        case JENKINS:      fprintf (stderr, "JENKINS\n");   return hash_JENKINS;
        case XXHASH:       fprintf (stderr, "XXHASH\n");    return hash_XXHASH;
        case CRC32_INLINE: fprintf (stderr, "CRC32INTR\n"); return hash_CRC32_inline;

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

char** hashT_fill (struct HashTable_t* hashT_ptr, const char* filename, int* num_words)
{
    assert (hashT_ptr);

    long num_symb = 0;
    char* buffer = MakeBuffer (filename, &num_symb);
    if (buffer == NULL)
        return NULL;

    char** words_arr = (char**) calloc ((size_t) num_symb, sizeof (char*));
    if (words_arr == NULL)
    {
        fprintf (stderr, "Error with filling words array\n");
        free (buffer);
        return NULL;
    }

    *num_words = 0;
    char* ptr = buffer;
    while (*ptr)
    {
        while (*ptr && !isalpha(*ptr))
        {
            *ptr = '\0';
            ptr++;
        }

        if (!*ptr) break;

        char* word = ptr;
        while (*ptr && (isalpha(*ptr)))
            ptr++;

        if (*ptr)
        {
            *ptr = '\0';
            ptr++;
        }

        char* word_cp = (char*) calloc (strlen (word) + 1, sizeof (char));
        if (word_cp == NULL)
        {
            fprintf (stderr, "ERROR with copying word\n");
            for (int i = 0; i < *num_words; i++)
                free (words_arr[i]);
            free (words_arr);
            free (buffer);
            return NULL;
        }

        strcpy (word_cp, word);

        uint32_t hash = 0;
        int status = hashT_fill_search (hashT_ptr, word, &hash);
        if (status)
            hashT_insert (hashT_ptr, word, &hash);

        words_arr[*num_words] = word_cp;
        (*num_words)++;
    }

    words_arr = realloc (words_arr, (size_t)*num_words * sizeof (char*));
    if (words_arr == NULL)
    {
        fprintf (stderr, "Error reallocating words array\n");
        for (int i = 0; i < *num_words; i++)
            free (words_arr[i]);
        free (words_arr);
        free (buffer);
        return NULL;
    }
    free (buffer);

    return words_arr;
}

int hashT_fill_search (struct HashTable_t* hashT_ptr, const char* data, uint32_t* hash)
{
    assert (hashT_ptr);
    assert (data);

    *hash = hashT_ptr->func_ptr (data);
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

    uint32_t hash = hashT_ptr->func_ptr (data);
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

int hashT_TEST (struct HashTable_t* hashT_ptr, char** words_arr, int num_words, size_t num_tests)
{
    int get = 0;

    for (size_t i = 0; i < num_tests; i++)
        for (int word_i = 0; word_i < num_words; word_i++)
        {
            get = hashT_search (hashT_ptr, words_arr[word_i]);

        }

    return get;
}
