#ifndef CHEESENG_LINKEDLIST_DEF_H
#define CHEESENG_LINKEDLIST_DEF_H

#include <stdlib.h>
#define LList(DATATYPE) LList_##DATATYPE

#define LListAppendNode(DATATYPE) LList_##DATATYPE ##_AppendNode
#define LListAppendData(DATATYPE) LList_##DATATYPE ##_AppendData
#define LListFreeNodes(DATATYPE) LList_##DATATYPE ##_FreeNodes
#define LListFreeData(DATATYPE) LList_##DATATYPE##_FreeData

#define LListFOR(DATATYPE, elem, list) \
for(Node_##DATATYPE *node = list.head; node && ((elem = node->data) || 1) ; node = node->next)

#define LListFORPTR(DATATYPE, elem, list) \
for(Node_##DATATYPE *node = list.head; node && ((elem = & (node->data)) || 1) ; node = node->next)


#define LListDeclarations(DATATYPE) \
typedef struct node_##DATATYPE \
{\
    struct node_##DATATYPE *prev;\
    struct node_##DATATYPE *next;\
\
    DATATYPE data;\
}Node_##DATATYPE;\
\
typedef struct LList_##DATATYPE\
{\
    Node_##DATATYPE *head, *tail;\
\
    unsigned int length;\
} LList_##DATATYPE;                 \
void LList_##DATATYPE##_AppendNode(LList(DATATYPE) *list, Node_##DATATYPE *node);\
void LList_##DATATYPE##_AppendData(LList(DATATYPE) *list, DATATYPE const data); \
void LList_##DATATYPE##_FreeNodes(LList(DATATYPE) * list);

#define LListDefinitions(DATATYPE) \
Node_##DATATYPE * CreateNode_##DATATYPE (DATATYPE const data)\
{\
    Node_##DATATYPE * ret = malloc(sizeof (Node_##DATATYPE));\
\
    ret->prev = NULL;\
    ret->next = NULL;\
\
    ret->data = data;\
\
    return ret;\
}\
\
void LList_##DATATYPE##_AppendNode(LList(DATATYPE) *list, Node_##DATATYPE *node)\
{\
    if(list->head == NULL)\
    {\
        list->head = node;\
        node->prev = NULL;\
        node->next = NULL;\
        list->tail = node;\
    }\
    else\
    {\
        Node_##DATATYPE *tmp = list->tail;\
        tmp->next = node;\
        node->prev = tmp;\
        node->next = NULL;\
        list->tail = node;\
    }\
\
    list->length++;\
}\
\
void LList_##DATATYPE##_AppendData(LList(DATATYPE) *list, DATATYPE const data) \
{\
    LListAppendNode(DATATYPE)(list, CreateNode_##DATATYPE (data));\
} \
\
void LList_##DATATYPE##_FreeNodes(LList(DATATYPE) * list) \
{ \
    for(Node_##DATATYPE *node = list->head, *tmpNode; node; node = tmpNode) \
    {\
        tmpNode = node->next;  \
        free(node);\
    }\
\
    list->head = NULL;\
    list->tail = NULL;\
\
    list->length = 0;\
}\

#define LListCreateFreeData(DATATYPE) \
void LList_##DATATYPE##_FreeData(LList(DATATYPE) * list) \
{ \
    for(Node_##DATATYPE *node = list->head, *tmpNode; node; node = tmpNode) \
    {\
        tmpNode = node->next;  \
        free(node);\
    }\
\
    list->head = NULL;\
    list->tail = NULL;\
\
    list->length = 0;\
}\

#define LListCreate(DATATYPE, name) LList_##DATATYPE name = (LList(DATATYPE)) { NULL, NULL, 0 }

#define LListDeclareDefine(DATATYPE) \
LListDeclarations(DATATYPE) \
LListDefinitions(DATATYPE)

#endif //CHEESENG_LINKEDLIST_DEF_H
