#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <immintrin.h>

#include "list.h"
#include "tools.h"

struct Node_t* node_insert (struct Node_t* parent, const char* data)
{
    assert (data);

    struct Node_t* new_node = (struct Node_t*) calloc (1, sizeof (struct Node_t));
    if (new_node == NULL)
    {
        ERROR_MESSAGE (NULL_PTR_ERR)
        return NULL;
    }

    char* word = calloc (MAX_LENGTH_WORD, sizeof (char));
    if (word == NULL)
    {
        ERROR_MESSAGE (NULL_PTR_ERR)
        return NULL;
    }

    memcpy (word, data, MAX_LENGTH_WORD);

    new_node->data = word;

    if (parent != NULL)
        parent->next = new_node;

    return new_node;
}

void list_dtor (struct Node_t* node)
{
    assert (node);

    while (node)
    {
        struct Node_t* f_free = node;
        node = node->next;

        free (f_free->data);
        free (f_free);
    }
}

struct Node_t* list_search (struct Node_t* node, const char* data)
{
    assert (node);
    assert (data);

    while (node)
    {
        if (boost_strcmp (node->data, data) == 0)
            return node;

        node = node->next;
    }

    return NULL;
}

int boost_strcmp (const char* str_1, const char* str_2)
{
    __m256i string_1 = _mm256_loadu_si256 ((const __m256i*)(str_1));
    __m256i string_2 = _mm256_loadu_si256 ((const __m256i*)(str_2));

    return ~(_mm256_movemask_epi8 (_mm256_cmpeq_epi8 (string_1, string_2)));
}
