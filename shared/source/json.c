#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "json.h"
#include "math.h"
#include "arraylist.h"

JSection *JDeserialiseSection()//int jLength, char *json)
{
    JSection *res = calloc(1, sizeof(JSection));
    res->count = 0;
    int capacity = 4;
    res->fields = calloc(capacity, sizeof(JField *));
    
    // some test to see if my stuff works
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
        LAppendArray((void ***)&(res->fields), &res->count, &capacity, sizeof(JField *), field);
    }
    
    return res;
}

char *JSerialiseValue(JValue *obj, int identLevel)
{
    if (obj->type == JTypeSection)
    {
        return JSerialiseSection(obj->ref, identLevel + 1);
    }
    else if (obj->type == JTypeString)
    {
        int jLength = 1 + strlen(obj->ref) + 1; // TODO: custom count length for escaping chars
        char *res = calloc(jLength + 1, sizeof(char));
        res[0] = '"';
        strcpy(res + 1, obj->ref); // TODO: escape
        res[jLength] = '"';
        return res;
    }
    else if (obj->type == JTypeInt32)
    {
        char *res = calloc(13, sizeof(char));
        sprintf(res, "%d", *(int*)(obj->ref));
        return res;
    }
    else if (obj->type == JTypeFloat64)
    {
        char *res = calloc(30, sizeof(char));
        sprintf(res, "%lf", *(double*)(obj->ref));
        return res;
    }
    else if (obj->type == JTypeBool)
    {
        char *res = calloc(6, sizeof(char));
        strcpy(res, *(bool*)(obj->ref) ? "true" : "false");
        return res;
    }
    else
    {
        printf("OH NO!");
        exit(-1);
    }
}

char *JSerialiseField(JField *obj, int identLevel)
{
    char *serialisedValue = JSerialiseValue(obj->value, identLevel);
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
    free(serialisedValue);
    return res;
}

char *JSerialiseSection(JSection *obj, int identLevel)
{
    int nrFields = obj->count;
    int fieldLengthsSum = 0;
    int fieldLengths[nrFields];
    char *serialisedFields[nrFields];
    for (int i = 0; i < nrFields; i++)
    {
        serialisedFields[i] = JSerialiseField(obj->fields[i], identLevel);
        fieldLengths[i] = strlen(serialisedFields[i]);
        fieldLengthsSum += fieldLengths[i];
    }
    //            fields            "\n" and ","                      identation for fields         identation behind "}"   "{" and "}"
    int jLength = fieldLengthsSum + (nrFields > 0 ? nrFields*2 : 1) + nrFields*(identLevel + 1)*4 + identLevel*4 +          2;
    char *res = calloc(jLength + 1, sizeof(char));
    char *resptr = res;
    *(resptr++) = '{';
    *(resptr++) = '\n';
    for (int i = 0; i < nrFields; i++)
    {
        for (int k = 0; k < identLevel + 1; k++)
        {
            strcpy(resptr, "    ");
            resptr += 4;
        }
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
