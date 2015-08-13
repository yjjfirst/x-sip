#include <stdio.h>
#include <string.h>

#include "Parser.h"

int FillTarget(char *value, void *target)
{
    strcpy(target, value);
}

int Parse(char *header, void* target, struct ParsePattern *pattern)
{
    char *curr = header;
    char *next = header;
    char value[128];
    
    for ( ; pattern->name != NULL;  pattern++) {
        if (!(curr = strstr(next, pattern->separator))) continue;
        bzero(value, sizeof(value));
        strncpy(value, next, curr - next);
        FillTarget(value, target + pattern->offset);
        next = curr + strlen(pattern->separator);
    }

    return 0;
}
