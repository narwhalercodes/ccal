#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "json.h"
#include "math.h"
#include "arraylist.h"

JSection *JDeserialiseSection(int jLength, char *json)
{
    JSection *res = calloc(1, sizeof(JSection));
    res->count = 0;
    int capacity = 4;
    res->fields = calloc(capacity, sizeof(JField *));
    
    // some test to see if my stuff works
    // this generates the following structure:
    //
    // {
    //     "a": "test0",
    //     "b": true,
    //     "c": "test2",
    //     "d": "test3",
    //     "nesty mac besty": {
    //         "a": "TEST_0",
    //         "b": "TEST_1",
    //         "c": "TEST_2",
    //         "d": "TEST_3",
    //         "e": "TEST_4"
    //     },
    //     "f": "test5",
    //     "_0": -0.225000,
    //     "_1": 1.575000,
    //     "_2": 11,
    //     "_3": 39,
    //     "_4": 111
    // }
    //
    for (int i = 0; i < 6; i++)
    {
        JField *field = calloc(1, sizeof(JField));
        field->value = calloc(1, sizeof(JValue));
        field->name = calloc(10, sizeof(char));
        field->name[0] = 'a' + i;
        field->value->type = JTypeString;
        field->value->size = 1;
        field->value->ref = calloc(10, sizeof(char));
        strcpy(field->value->ref, "testx");
        ((char *)(field->value->ref))[4] = '0' + i;
        LAppendArray((void ***)(&(res->fields)), &(res->count), &capacity, sizeof(JField *), field);
    }
    for (int i = 0; i < 2; i++)
    {
        JField *field = calloc(1, sizeof(JField));
        field->value = calloc(1, sizeof(JValue));
        field->name = calloc(10, sizeof(char));
        field->name[0] = '_';
        field->name[1] = '0' + i;
        field->value->type = JTypeFloat64;
        field->value->size = 1;
        field->value->ref = calloc(1, sizeof(double));
        *(double *)(field->value->ref) = 1.5 * (i - 0.3) * (i + 0.5);
        LAppendArray((void ***)(&(res->fields)), &(res->count), &capacity, sizeof(JField *), field);
    }
    for (int i = 2; i < 5; i++)
    {
        JField *field = calloc(1, sizeof(JField));
        field->value = calloc(1, sizeof(JValue));
        field->name = calloc(10, sizeof(char));
        field->name[0] = '_';
        field->name[1] = '0' + i;
        field->value->type = JTypeInt32;
        field->value->size = 1;
        field->value->ref = calloc(1, sizeof(int));
        *(int *)(field->value->ref) = (1 << i)*(2*i - 1) - 1;
        LAppendArray((void ***)(&(res->fields)), &(res->count), &capacity, sizeof(JField *), field);
    }

    free(res->fields[1]->value->ref);
    res->fields[1]->value->type = JTypeBool;
    res->fields[1]->value->size = 1;
    res->fields[1]->value->ref = calloc(1, sizeof(bool));
    *(bool *)(res->fields[1]->value->ref) = true;

    JFreeValue(res->fields[4]->value);
    res->fields[4]->value = calloc(1, sizeof(JValue));
    res->fields[4]->name = calloc(40, sizeof(char));
    strcpy(res->fields[4]->name, "nesty mac besty");
    res->fields[4]->value->type = JTypeSection;
    int capacity2 = 1;
    res->fields[4]->value->ref = calloc(1, sizeof(JSection));
    JSection *innerSection = res->fields[4]->value->ref;
    innerSection->fields = calloc(capacity2, sizeof(JField *));
    innerSection->count = 0;
    for (int i = 0; i < 5; i++)
    {
        JField *field = calloc(1, sizeof(JField));
        field->value = calloc(1, sizeof(JValue));
        field->name = calloc(10, sizeof(char));
        field->name[0] = 'a' + i;
        field->value->type = JTypeString;
        field->value->size = 1;
        field->value->ref = calloc(10, sizeof(char));
        strcpy(field->value->ref, "TEST_x");
        ((char *)(field->value->ref))[5] = '0' + i;
        LAppendArray((void ***)(&(innerSection->fields)), &(innerSection->count), &capacity2, sizeof(JField *), field);
    }
    res->fields[4]->value->size = innerSection->count;
    
    return res;
}

char *JSerialiseValue(JValue *obj, int identLevel)
{
    if (identLevel > 64)
    {
        printf("EXCEEDED 64 NESTINGS\n");
        return NULL;
    }
    //printf("VALUE BEGIN %d, %d, %d\n", obj->type, obj->size, (int)obj->ref);
    if (obj->type == JTypeSection)
    {
        char *res = JSerialiseSection(obj->ref, identLevel);
        //printf("VALUE END\n");
        return res;
    }
    else if (obj->type == JTypeString)
    {
        int len = strlen(obj->ref);
        int jLength = 1 + len + 1; // TODO: custom count length for escaping chars
        char *res = calloc(jLength + 1, sizeof(char));
        res[0] = '"';
        strcpy(res + 1, obj->ref); // TODO: escape
        res[1 + len] = '"';
        //printf("VALUE END\n");
        return res;
    }
    else if (obj->type == JTypeInt32)
    {
        char *res = calloc(13, sizeof(char));
        sprintf(res, "%d", *(int*)(obj->ref));
        //printf("VALUE END\n");
        return res;
    }
    else if (obj->type == JTypeFloat64)
    {
        char *res = calloc(30, sizeof(char));
        sprintf(res, "%lf", *(double*)(obj->ref));
        //printf("VALUE END\n");
        return res;
    }
    else if (obj->type == JTypeBool)
    {
        char *res = calloc(6, sizeof(char));
        strcpy(res, *(bool*)(obj->ref) ? "true" : "false");
        //printf("VALUE END\n");
        return res;
    }
    else
    {
        printf("OH NO!");
        return NULL;
    }
}

char *JSerialiseField(JField *obj, int identLevel)
{
    //printf("FIELD BEGIN %d\n", (int)obj->name);
    //printf("%s\n", obj->name);
    //printf("%d\n", (int)obj->value);

    char *serialisedValue = JSerialiseValue(obj->value, identLevel);
    if (serialisedValue == NULL)
        return NULL;
    int len1 = strlen(obj->name); // TODO: custom count length for escaping chars
    int len2 = strlen(serialisedValue);
    int jLength = identLevel*4 + 1 + len1 + 3 + len2;
    char *res = calloc(jLength + 1, sizeof(char));
    for (int i = 0; i < identLevel; i++)
        strcpy(res + i, "    ");
    res[identLevel*4] = '"';
    strcpy(res + identLevel*4 + 1, obj->name); // TODO: escape
    strcpy(res + identLevel*4 + 1 + len1, "\": ");
    strcpy(res + identLevel*4 + 1 + len1 + 3, serialisedValue);
    //printf("FIELD END\n");
    free(serialisedValue);
    return res;
}

char *JSerialiseSection(JSection *obj, int identLevel)
{
    //printf("SECTION BEGIN %d", obj->count);
    //for (int i = 0; i < obj->count; i++)
    //{
    //    printf(" %d", (int)obj->fields[i]);
    //}
    //printf("\n");

    int nrFields = obj->count;
    int fieldLengthsSum = 0;
    int fieldLengths[nrFields];
    char *serialisedFields[nrFields];
    for (int i = 0; i < nrFields; i++)
    {
        serialisedFields[i] = JSerialiseField(obj->fields[i], identLevel + 1);
        if (serialisedFields[i] == NULL)
            return NULL;
        fieldLengths[i] = strlen(serialisedFields[i]);
        fieldLengthsSum += fieldLengths[i];
    }
    //            fields            "\n" and ","                     identation behind "}"   "{" and "}"
    int jLength = fieldLengthsSum + (nrFields > 0 ? nrFields*2 : 1) + identLevel*4 +          2;
    char *res = calloc(jLength + 1, sizeof(char));
    char *resptr = res;
    *(resptr++) = '{';
    *(resptr++) = '\n';
    for (int i = 0; i < nrFields; i++)
    {
        strcpy(resptr, serialisedFields[i]);
        resptr += fieldLengths[i];
        if (i != nrFields - 1)
        {
            *(resptr++) = ',';
        }
        *(resptr++) = '\n';
    }
    for (int k = 0; k < identLevel; k++)
    {
        strcpy(resptr, "    ");
        resptr += 4;
    }
    *(resptr++) = '}';
    *(resptr++) = '\0';
    for (int i = 0; i < nrFields; i++)
    {
        free(serialisedFields[i]);
    }
    
    //printf("SECTION END\n");
    return res;
}

void JFreeValue(JValue *obj)
{
    if (obj->type == JTypeSection)
    {
        JFreeSection(obj->ref);
    }
    else
    {
        free(obj->ref);
    }
    free(obj);
}

void JFreeField(JField *obj)
{
    JFreeValue(obj->value);
    free(obj->name);
    free(obj);
}

void JFreeSection(JSection *obj)
{
    for (int i = 0; i < obj->count; i++)
    {
        JFreeField(obj->fields[i]);
    }
    free(obj);
}
