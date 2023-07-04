#ifndef CCAL_JSON_H
#define CCAL_JSON_H

// JSection *   size = ((JSection*)ref)->count
#define JTypeSection 0

// char *       size = strlen(ref)
#define JTypeString 1

// int *        size = 1
#define JTypeInt32 2

// double *     size = 1
#define JTypeFloat64 3

// bool *       size = 1
#define JTypeBool 4

typedef struct JValue {
    int type;
    int size;
    void *ref;
} JValue;

typedef struct JField {
    char *name;
    JValue *value;
} JField;

typedef struct JSection {
    int count;
    JField **fields;
} JSection;

JSection *JDeserialiseSection(int jLength, char *json);

char *JSerialiseValue(JValue *obj, int identLevel);
char *JSerialiseField(JField *obj, int identLevel);
char *JSerialiseSection(JSection *obj, int identLevel);

void JFreeValue(JValue *obj);
void JFreeField(JField *obj);
void JFreeSection(JSection *obj);

#endif
