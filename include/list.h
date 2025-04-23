#ifndef LIST_H
#define LIST_H

#define RIGHT_MASK 0xFFFFFFFF

struct Node_t
{
    char* data;
    struct Node_t* next;
};

struct Node_t* node_insert (struct Node_t* parent, const char* data);

void list_dtor (struct Node_t* node);

struct Node_t* list_search (struct Node_t* node, const char* data);

int boost_strcmp (const char *str_1, const char *str_2);

#endif // LIST_H
