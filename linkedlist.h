#ifndef CHEESENG_LINKEDLIST_H
#define CHEESENG_LINKEDLIST_H

#include <stdlib.h>

#define DATATYPE int

typedef struct node
{
    struct node *prev;
    struct node *next;

    DATATYPE data;
}Node;

typedef struct LList
{
    Node *head, *tail;

    unsigned int length;
} LList;

Node *CreateNode(const DATATYPE data)
{
    Node * ret = malloc(sizeof (Node));

    ret->prev = NULL;
    ret->next = NULL;

    ret->data = data;

    return ret;
}

void LListAppendNode(LList *list, Node *node)
{
    if(list->head == NULL)
    {
        list->head = node;
        node->prev = NULL;
        node->next = NULL;
        list->tail = node;
    }
    else
    {
        Node *tmp = list->tail;
        tmp->next = node;
        node->prev = tmp;
        node->next = NULL;
        list->tail = node;
    }

    list->length++;
}

void LListAppendData(LList *list, const DATATYPE data)
{
    LListAppendNode(list, CreateNode(data));
}



#define LListFOR(elem, list) \
Node * tmpNext;\
DATATYPE elem; \
for( Node * node = list.head ; node && ( tmpNext = node->next, elem = node->data ); node = tmpNext)

void LListFree(LList * list)
{
    LListFOR(elem, (*list))
    {
        free(node);
    }

    list->head = NULL;
    list->tail = NULL;

    list->length = 0;
}

#define LListCreate(name) LList name = (LList) { NULL, NULL, 0 }


#endif //CHEESENG_LINKEDLIST_H
