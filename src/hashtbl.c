#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "hashtbl.h"
#include "file.h"
#include "list.h"
#include "tools.h"

uint32_t DJB2_hash (const char* str)
{
    uint32_t hash = 5381;
    unsigned char c;

    while ((c = (unsigned char) *str++) != 0)
        hash = (((hash << 5) + hash) + (uint32_t) c);

    return hash;
}

struct HashTable_t* hashT_ctor (long size)
{
    struct HashTable_t* hsh_t = calloc (1, sizeof (struct HashTable_t));
    if (hsh_t == NULL)
    {
        ERROR_MESSAGE (NULL_PTR_ERR)
        return NULL;
    }

    hsh_t->buckets = calloc ((size_t) size, sizeof (struct Node_t*));
    if (hsh_t->buckets == NULL)
    {
        ERROR_MESSAGE (NULL_PTR_ERR)
        return NULL;
    }

    hsh_t->size = size;

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

    long numb_symb = 0;
    char* buffer = MakeBuffer (filename, &numb_symb);
    char* old_buffer = buffer;

    char* ptr = buffer;
    while (*ptr)
    {
        while (*ptr && !isalpha(*ptr) && !isdigit(*ptr))
        {
            *ptr = '\0';
            ptr++;
        }

        if (!*ptr) break;

        char* word = ptr;
        while (*ptr && (isalpha(*ptr) || isdigit(*ptr)))
            ptr++;

        if (*ptr)
        {
            *ptr = '\0';
            ptr++;
        }

        uint32_t hash = 0;
        int status = hashT_search (hashT_ptr, word, &hash);
        if (status)
            hashT_insert (hashT_ptr, word, &hash);

        // maybe increments some value
    }

    free (old_buffer);

    return 0;
}

int hashT_search (struct HashTable_t* hashT_ptr, const char* data, uint32_t* hash)
{
    assert (hashT_ptr);
    assert (data);

    *hash = DJB2_hash (data);
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
