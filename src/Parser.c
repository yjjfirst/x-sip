#include <stdio.h>
#include <string.h>

#include "Parser.h"

int FillTarget(char *value, void *target)
{
    strcpy(target, value);
}

char *NextToken (char *header)
{
    char tokens[] = {SPACE, SEMICOLON, COLON, AT, QUESTION, };
    int i = 0;
    int tokenCount = sizeof(tokens) / sizeof (char);
    char *pos = header;

    for (; *pos != 0; pos ++) {
        for (i = 0; i < tokenCount; i ++) {
            if (*pos == tokens[i]) {
                return pos;
            }                                                             
        }
    }

    return header + strlen(header);
}

char *FindEndToken(char *header, char token)
{
    if (token == ANY)
        return NextToken(header);
    
    return strchr(header, token);
}

char *FindNextComponent(char *header, struct ParsePattern *pattern)
{
    char *curr = NULL;
    char *nextToken = NULL;

    if (pattern->startToken == EMPTY) {
        curr = strchr(header, pattern->endToken);
        return curr;
    }
    
    if (pattern->startToken == header[0]) {
        curr = FindEndToken (header + 1, pattern->endToken);
        if (curr == NULL) curr = header;
    }
    else {
        curr = header;
    }

    return curr;
}

char *SkipToken(char *original, char *current) 
{
    char *start = NULL;
    
    if (original == current)
        start = current ;
    else
        start = current + 1;
    
    return start;
} 

int Parse(char *header, void* target, struct ParsePattern *pattern)
{
    char *curr = header;
    char *next = header;
    char value[128];
    char *start = NULL;
    
    for ( ; pattern->name != NULL;  pattern++) {
            
        curr = FindNextComponent(next, pattern);
        bzero(value, sizeof(value));
        start = SkipToken(header, next);
        if (curr - start >= 0)            
            strncpy(value, start , curr - start);

        FillTarget(value, target + pattern->offset);
        next = curr;
    }

    return 0;
}
