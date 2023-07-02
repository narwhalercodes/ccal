#include <strings.h>
#include <stdbool.h>
#ifndef CCAL_JSON_H
    #include "json.h"
#endif
#ifndef CCAL_MATH_H
    #include "math.h"
#endif

JSection *JDeserialiseSection(int jLength, char *json)
{
    JSection *res = calloc(1, sizeof(JSection));
    res->count = 0;
    res->fields = calloc(8, sizeof(JField*));
    res->fields[0] = calloc(1, sizeof(JField));
    res->fields[0]->type = JTypeString;
    res->fields[0]->size = 1;
    res->fields[0]->ref = calloc(10, sizeof(char));
    strcpy(res->fields[0]->ref, "test");
    return res;
}

char *JSerialiseValue(JValue *obj, int identLevel)
{
    if (obj->type == JValueSection)
    {
        return JSerialiseSection(obj, identLevel + 1);
    }
    else if (obj->type == JValueString)
    {
        jLength = 1 + strlen(obj->ref) + 1; // TODO: custom count length for escaping chars
        char *res = calloc(jLength + 1, sizeof(char));
        res[0] = '"';
        strcpy(res + 1, obj->ref); // TODO: escape
        res[jLength] = '"';
        return res;
    }
    else if (obj->type == JValueInt32)
    {
        char *res = calloc(13, sizeof(char));
        sprintf(res, "%d", *(int*)(obj->ref))
        return res;
    }
    else if (obj->type == JValueFloat64)
    {
        char *res = calloc(30, sizeof(char));
        sprintf(res, "%llf", *(double*)(obj->ref))
        return res;
    }
    else if (obj->type == JValueBool)
    {
        char *res = calloc(6, sizeof(char));
        strcpy(res, *(bool*)(obj->ref) ? "true" : "false");
        return res;
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
    int jLength = FieldLengthsSum + (nrFields > 0 ? nrFields*2 : 1) + nrFields*(identLevel + 1)*4 + identLevel*4 +          2;
    char *res = calloc(jLength + 1, sizeof(char));
    char *resptr = res;
    &resptr++ = '{';
    &resptr++ = '\n';
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
            &resptr++ = ',';
        }
        &resptr++ = '\n';
    }
    for (int k = 0; k < identLevel; k++)
    {
        strcpy(resptr, "    ");
        resptr += 4;
    }
    &resptr++ = '}';
    &resptr++ = '\0';
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
