#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Parser.h"

int ParseStringElement(char *value, void *target)
{
    char *start = value;
    char length = strlen(value);
    char *end = value + length - 1;
    
    if (length == 0) {
        strcpy(target, value);
        return 0;
    }
    while (*start == SPACE) start ++;
    while (*end == SPACE) end--;

    strncpy(target, start, end - start + 1);
    return 0;
}

int ParseIntegerElement(char *value, void *target)
{
    int *t = (int *) target;
    *t = atoi(value);
    
    return 0;
}

char *NextToken (char *header)
{
    char tokens[] = {SPACE, SEMICOLON, COLON, AT, QUESTION, QUOTE};
    int i = 0;
    int tokenCount = sizeof(tokens) / sizeof (char);
    char *pos = header;

    //printf("%s\n", header);
    for (; *pos != 0; pos ++) {
        for (i = 0; i < tokenCount; i ++) {
            if (*pos == tokens[i]) {
                return pos;
            }                                                             
        }
    }

    return header + strlen(header);
}

char *FindEndToken(char *header, struct ParsePattern *pattern)
{
    char *format = pattern->format;

    while (*header == SPACE) header ++;

    if (pattern->endToken == ANY)
        return NextToken(header);

    if (*format == '*')
        return strchr(header, pattern->endToken);

    for ( ; *format != 0; format++) {
        header =  strchr (header, *format);        
    }

    header ++;
    return strchr(header, pattern->endToken);
}

char *FindNextComponent(char *header, struct ParsePattern *pattern)
{
    char *next = NULL;

    if (pattern->startToken == EMPTY) {
        next = strchr(header, pattern->endToken);
        return next;
    }
        
    if (pattern->startToken == header[0]) {
        next = FindEndToken (header + 1, pattern);
        if (next == NULL) next = header;
    }
    else {
        next = header;
    }

    return next;
}

char *SkipFirstToken(char *header, char *position) 
{
    char *start = NULL;
    
    if (header == position)
        start = position ;
    else
        start = position + 1;
    
    return start;
} 

int Parse(char *header, void* target, struct ParsePattern *pattern)
{
    char *curr;
    char *position;
    char value[128];
    char *start = NULL;
    
    if (pattern == NULL) {
        return -1;
    }
    
    while (*header == SPACE) header ++;
    curr = position = header;

    for ( ; pattern->format != NULL;  pattern++) {            
        curr = FindNextComponent(position, pattern);
        bzero(value, sizeof(value));
        start = SkipFirstToken(header, position);
        if (curr - start >= 0) {         
            strncpy(value, start , curr - start);
        }

        if (!pattern->placeholder) {
            pattern->Parse(value, target + pattern->offset);
        }

        position = curr;
    }

    return 0;
}
