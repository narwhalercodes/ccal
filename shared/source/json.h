#ifndef CCAL_JSON_H
#define CCAL_JSON_H

#define JTypeSection 0      // JSection *   size = ((JSection*)ref)->count
#define JTypeString 1       // char *       size = strlen(ref)
#define JTypeInt32 2        // int *        size = 1
#define JTypeFloat64 3      // double *     size = 1
#define JTypeBool 4         // bool *       size = 1

typedef struct JValue {
    int type,
    int size,
    void *ref
} JValue;

typedef struct JField {
    char *name,
    JValue *value
} JField;

typedef struct JSection {
    int count,
    **JField fields
} JSection;

JSection *JDeserialiseSection(int jLength, char *json);

char *JSerialiseValue(JValue *obj);
char *JSerialiseField(JField *obj);
char *JSerialiseSection(JSection *obj);

void JFreeValue(JValue *obj);
void JFreeField(JField *obj);
void JFreeSection(JSection *obj);

#endif
