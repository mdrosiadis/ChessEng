#ifndef CHEESENG_DARRAY_DEF_H
#define CHEESENG_DARRAY_DEF_H

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define Darray(T) Darray_##T
#define DarrayInit(T) Darray_##T##_Init
#define DarrayRealloc(T) Darray_##T##_Realloc
#define DarrayPush(T) Darray_##T##_Push
#define DarrayExtend(T) Darrray_##T##_Extend
#define DarrayFree(T) Darray_##T##_Free

#define DarrayDeclarations(T) \
typedef struct darray_##T\
{\
    T * data;\
\
    unsigned int capacity, length;\
} Darray(T);\
void DarrayRealloc(T)(Darray(T) *arr, unsigned int sizeInElems);\
Darray(T) DarrayInit(T)(unsigned int initialCapacity);\
void DarrayPush(T)(Darray(T) *arr, T elem);\
void DarrayExtend(T)(Darray(T) *dest, const Darray(T) *from);\
void DarrayFree(T)(Darray(T) *arr);



#define DarrayDefinitions(T) \
void DarrayRealloc(T)(Darray(T) *arr, unsigned int sizeInElems)\
{\
    arr->data = realloc(arr->data, sizeInElems * sizeof (T));\
\
\
\
    arr->capacity = sizeInElems;\
}\
\
Darray(T) DarrayInit(T)(unsigned int initialCapacity)\
{\
    Darray(T) ret;\
\
    ret.data = NULL;\
    ret.length = 0;\
\
    DarrayRealloc(T)(&ret, initialCapacity);\
\
    return ret;\
}\
\
void DarrayPush(T)(Darray(T) *arr, T elem)\
{\
   if(arr->length >= arr->capacity) DarrayRealloc(T)(arr, arr->capacity * 2);\
\
   arr->data[arr->length++] = elem;\
}\
\
void DarrayExtend(T)(Darray(T) *dest, const Darray(T) *from)\
{\
    unsigned int leastNewCap = dest->length + from->length;\
\
    if(dest->capacity <= leastNewCap) DarrayRealloc(T)(dest, leastNewCap);\
\
    memcpy(dest->data + dest->length, from->data, from->length * sizeof (T));\
\
    dest->length += from->length;\
}\
\
void DarrayFree(T)(Darray(T) *arr)\
{\
    free(arr->data);\
\
    arr->data = NULL;\
    arr->length = 0; arr->capacity = 0;\
}

#endif //CHEESENG_DARRAY_DEF_H
