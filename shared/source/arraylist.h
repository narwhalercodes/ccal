#ifndef CCAL_ARRAYLIST_H
#define CCAL_ARRAYLIST_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "math.h"

typedef struct List {
    int count;
    int capacity;
    size_t bytesPerElement;
    void **elems;
} List;

#define NEW_LIST(T, n) {                   \
    int N = (n);                           \
    List *list = calloc(1, sizeof(List));  \
    list->count = 0;                       \
    list->capacity = N;                    \
    list->bytesPerElement = sizeof(T);     \
    list->elems = calloc(N, sizeof(T));    \
    list;                                  \
}

#define LIST_FROM_ARRAY(arr, T, n) {       \
    T *ARR = (void *)(arr);                \
    int N = (n);                           \
    int C = MIN(150000, MAX(8, N*2));      \
    List *list = NEW_LIST(T, C);           \
    list->capacity = C;                    \
    list->count = N;                       \
    for (int i = 0; i < C; i++)            \
    {                                      \
        list->elems[i] = ARR[i];           \
    }                                      \
    list;                                  \
}

void LAppendArray(void ***arr, int *count, int *capacity, size_t bytesPerElement, void *value);
void *LRemoveLastArray(void **arr, int *count);
void LAppend(List *list, void *value);
void *LRemoveLast(List *list);
bool LAny(List *list);

#endif
