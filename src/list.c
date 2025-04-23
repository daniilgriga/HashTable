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

    size_t data_len = strlen (data);

    char* word = calloc (data_len + 1, sizeof (char));
    if (word == NULL)
    {
        ERROR_MESSAGE (NULL_PTR_ERR)
        return NULL;
    }

    memcpy (word, data, data_len);

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
        if (strcmp (node->data, data) == 0)
            return node;

        node = node->next;
    }

    return NULL;
}

int boost_strcmp (const char *str_1, const char *str_2)
{
    __m128i s1 = _mm_loadu_si128 ((const __m128i*)str_1);
    __m128i s2 = _mm_loadu_si128 ((const __m128i*)str_2);

    __m128i eq = _mm_cmpeq_epi8 (s1, s2);

    __m128i zero  = _mm_setzero_si128();
    __m128i term1 = _mm_cmpeq_epi8 (s1, zero);

    int mask_eq = _mm_movemask_epi8 (eq);
    int mask_term1 = _mm_movemask_epi8 (term1);

    if (mask_eq != 0xFFFF)
    {
        int pos = __builtin_ctz (~mask_eq);                     // `~` = complement of the original number
        if (mask_term1 & (1 << pos))
            return 0;

        return (unsigned char)str_1[pos] - (unsigned char)str_2[pos];
    }

    if (mask_term1)
        return 0;

    str_1 += 16;
    str_2 += 16;
    while (*str_1 && *str_1 == *str_2)
    {
        str_1++;
        str_2++;
    }
    return (unsigned char)*str_1 - (unsigned char)*str_2;
}
