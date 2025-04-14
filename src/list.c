#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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

    memcpy (word, data, data_len + 1);

    new_node->data = word;
    new_node->next = NULL;

    if (parent != NULL)
        parent->next = new_node;

    return new_node;
}

int list_dtor (struct Node_t* node)
{
    assert (node);

    struct Node_t* curr = node;

    while (curr)
    {
        struct Node_t* f_free = curr;
        curr = curr->next;

        free (f_free->data);
        free (f_free);
    }

    return NO_ERRORS;
}

int list_search (struct Node_t* node, const char* data)
{
    assert (node);
    assert (data);

    struct Node_t* curr = node;
    while (curr != NULL)
    {
        if (strcmp (curr->data, data) == 0)
            return 1;

        curr = curr->next;
    }

    return 0;
}
