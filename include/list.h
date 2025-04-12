#ifndef LIST_H
#define LIST_H

struct Node_t
{
    const char* data;
    struct Node_t* next;
};

struct Node_t* node_insert (const char* data);

int list_dtor (struct Node_t* node);

#endif // LIST_H
