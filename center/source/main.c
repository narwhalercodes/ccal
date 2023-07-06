#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../../shared/source/json.h"
#include "../../shared/source/sfuncs.h"

SContainer *SContainerFromString(int chunkSize, int size, char *s);
void test1(void);
void test2(void);
void test3(void);
void test4(void);
void test5(void);
void test6(void);
void test7(void);

int main()
{
    // char json[] = "";
    // JSection *section = JDeserialiseSection(strlen(json), json);
    // printf("yo.\n");
    // printf("%d\n", section->count);
    // char *serialised = JSerialiseSection(section, 0);
    // printf("strlen(serialised) = %d\n", (int)strlen(serialised));
    // printf("%s\n", serialised);
    // JFreeSection(section);
    // free(serialised);
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();
}

SContainer *SContainerFromString(int chunkSize, int size, char *s)
{
    SContainer *res = calloc(1, sizeof(SContainer));
    res->chunkSize = chunkSize;
    res->size = size;
    res->length = 0;
    res->chunks = calloc(size, sizeof(char *));
    char c;
    for (int i = 0; i < size; i++)
    {
        c = *s;
        if (c == '\0')
        {
            return res;
        }
        res->chunks[i] = calloc(chunkSize + 1, sizeof(char));
        for (int k = 0; k < chunkSize; k++)
        {
            res->chunks[i][k] = c = *(s++);
            if (c == '\0')
            {
                return res;
            }
            res->length++;
        }
    }
    return res;
}

void test1()
{
    SContainer *s1 = SContainerFromString(8, 10, "yo");
    SContainer *s2 = SContainerFromString(8, 10, "he");
    int res;
    int r = SReadChars(s1, (SPointer){ 0, 0 }, s2, (SPointer){ 0, 2 }, NULL, 20, false, &res);
    char **c2 = s2->chunks;
    printf("%d, %d, %d, %c%c%c%c 0x%02X\n", r, res, s2->length, c2[0][0], c2[0][1], c2[0][2], c2[0][3], (int)c2[0][4]);
    SContainerFree(s1);
    SContainerFree(s2);
}

void test2()
{
    SContainer *s1 = SContainerFromString(8, 10, " mighty! welcome to the crew.");
    SContainer *s2 = SContainerFromString(8, 10, "hello there");
    int res;
    int r = SReadChars(s1, (SPointer){ 0, 0 }, s2, (SPointer){ 1, 1 }, NULL, 20, false, &res);
    char **c2 = s2->chunks;
    printf("%d, %d, %d, %s%s%s%s %d\n", r, res, s2->length, c2[0], c2[1], c2[2], c2[3], c2[4] == NULL);
    SContainerFree(s1);
    SContainerFree(s2);
}

void test3()
{
    SContainer *s1 = SContainerFromString(8, 10, " mighty! welcome to the crew.");
    SContainer *s2 = SContainerFromString(8, 10, "hello there");
    int res;
    int r = SReadChars(s1, (SPointer){ 0, 0 }, s2, (SPointer){ 1, 1 }, NULL, 50, false, &res);
    char **c2 = s2->chunks;
    printf("%d, %d, %d, %s%s%s%s%s\n", r, res, s2->length, c2[0], c2[1], c2[2], c2[3], c2[4]);
    SContainerFree(s1);
    SContainerFree(s2);
}

void test4()
{
    SContainer *s1 = SContainerFromString(8, 10, " mighty! welcome to the crew.");
    SContainer *s2 = SContainerFromString(8, 10, "hello there");
    int res;
    int r = SReadChars(s1, (SPointer){ 0, 0 }, s2, (SPointer){ 1, 1 }, NULL, -1, false, &res);
    char **c2 = s2->chunks;
    printf("%d, %d, %d, %s%s%s%s%s\n", r, res, s2->length, c2[0], c2[1], c2[2], c2[3], c2[4]);
    SContainerFree(s1);
    SContainerFree(s2);
}

void test5()
{
    SContainer *s1 = SContainerFromString(8, 10, " mighty! welcome to the crew.");
    SContainer *s2 = SContainerFromString(8, 10, "hello there");
    int res;
    int r = SReadChars(s1, (SPointer){ 0, 0 }, s2, (SPointer){ 1, 1 }, NULL, 29, false, &res);
    char **c2 = s2->chunks;
    printf("%d, %d, %d, %s%s%s%s%s\n", r, res, s2->length, c2[0], c2[1], c2[2], c2[3], c2[4]);
    SContainerFree(s1);
    SContainerFree(s2);
}

void test6()
{
    SContainer *s1 = SContainerFromString(8, 10, " mighty! welcome to the crew.");
    SContainer *s2 = SContainerFromString(8, 10, "hello there");
    s1->chunks[3][2] = '\0';
    int res;
    int r = SReadChars(s1, (SPointer){ 0, 0 }, s2, (SPointer){ 1, 1 }, NULL, -1, true, &res);
    char **c2 = s2->chunks;
    printf("%d, %d, %d, %s%s%s%s%s\n", r, res, s2->length, c2[0], c2[1], c2[2], c2[3], c2[4]);
    SContainerFree(s1);
    SContainerFree(s2);
}

void test7()
{
    SContainer *s1 = SContainerFromString(8, 10, " mighty! welcome to the crew.");
    SContainer *s2 = SContainerFromString(5, 5, "hello there");
    int res;
    int r = SReadChars(s1, (SPointer){ 0, 0 }, s2, (SPointer){ 1, 4 }, NULL, -1, false, &res);
    char **c2 = s2->chunks;
    printf("%d, %d, %d, %s%s%s%s%s\n", r, res, s2->length, c2[0], c2[1], c2[2], c2[3], c2[4]);
    SContainerFree(s1);
    SContainerFree(s2);
}
