#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "arraylist.h"
#include "math.h"

void LAppendArray(void ***arr, int *count, int *capacity, size_t bytesPerElement, void *value)
{
    if (*count == *capacity)
    {
        int newCapacity = *capacity < 1 ? 1 : MIN(*capacity * 2, 150000);
        if (newCapacity <= *capacity)
        {
            printf("oh no.");
            exit(-1);
        }
        void **newArr = calloc(newCapacity, bytesPerElement);
        memcpy(newArr, *arr, bytesPerElement * (*count));
        memcpy(((char *)newArr) + bytesPerElement * (*count), value, bytesPerElement);
        free(*arr);
        *arr = newArr;
        count++;
        *capacity = newCapacity;
    }
    else
    {
        memcpy(((char *)*arr) + bytesPerElement * (*count), value, bytesPerElement);
        count++;
    }
}

void *LRemoveLastArray(void **arr, int *count)
{
    if (count == 0)
    {
        return (void *)0;
    }
    void *lastValue = arr[count];
    arr[count--] = (void *)0;
    return lastValue;
}

void LAppend(List *list, void *value)
{
    AppendArray(&list->elems, &list->count, &list->capacity, list->bytesPerElement, value);
}

void *LRemoveLast(List *list)
{
    return LRemoveLastArray(list->elems, &list->count);
}

bool LAny(List *list)
{
    return list->count ? 1 : 0;
}
