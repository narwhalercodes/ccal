#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../shared/source/json.h"

int main(void)
{
    char json[] = "";
    JSection *section = JDeserialiseSection(strlen(json), json);
    printf("yo.\n");
    printf("%d\n", section->count);
    char *serialised = JSerialiseSection(section, 0);
    printf("%s\n", serialised);
    JFreeSection(section);
    free(serialised);
}
