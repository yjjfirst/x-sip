#include <stdio.h>
#include <string.h>

#include "Parser.h"

int FillTarget(char *value, void *target)
{
    strcpy(target, value);
}

int HasNextToken (char *string)
{
    return strstr(string, SPACE) ||
        strstr(string, SEMICOLON) ||
        strstr(string, COLON) ||
        strstr(string, AT) ||
        strstr(string, QUESTION);
}

char *FindNextToken(char *string, char *token, int last)
{
    char *curr = NULL;

    if (!(curr = strstr(string, token))) 
    {
        if (HasNextToken(string)) {
            curr = string;
        } else {
            curr = string + strlen(string);
        }        
        return curr;
    }
    
    if (curr == string) {
        curr = string + strlen(string);
    }
    return curr;
}

int Parse(char *header, void* target, struct ParsePattern *pattern)
{
    char *curr = header;
    char *next = header;
    char value[128];
    
    for ( ; pattern->name != NULL;  pattern++) {

        curr = FindNextToken(next, pattern->token, pattern->maybe_last);
        bzero(value, sizeof(value));
        strncpy(value, next, curr - next);

        FillTarget(value, target + pattern->offset);
        next = curr + strlen(pattern->token);
    }

    return 0;
}
