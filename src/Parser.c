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

char *NextSeparator (char *header)
{
    char tokens[] = {SPACE, SEMICOLON, COLON, AT, QUESTION, QUOTE};
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

char *FindEndSeparator(char *header, struct ParsePattern *pattern)
{
    char *format = pattern->format;

    while (*header == SPACE) header ++;

    if (pattern->endSeparator == ANY)
        return NextSeparator(header);

    if (*format == '*') {
        char *end = strchr(header, pattern->endSeparator);
        if (end == NULL && pattern->mandatory) {
            end = header + strlen (header);            
        }
        
        return end;
    }

    for ( ; *format != 0; format++) {
        header =  strchr (header, *format);        
    }

    header ++;
    return strchr(header, pattern->endSeparator);
}

char *FindNextComponent(char *header, struct ParsePattern *pattern)
{
    char *end = NULL;

    if (pattern->startSeparator == EMPTY) {
        end = strchr(header, pattern->endSeparator);
        return end;
    }
        
    if (pattern->startSeparator == header[0]) {
        end = FindEndSeparator (header + 1, pattern);
        if (end == NULL) end = header;
    }
    else {
        end = header;
    }

    return end;
}

char *SkipLeadingSeparator(char *header, char *position) 
{
    char *start = NULL;
    
    if (header == position)
        start = position ;
    else
        start = position + 1;
    
    return start;
} 

#define MAX_ELEMENT_LENGTH 1024
int Parse(char *header, void* target, struct ParsePattern *pattern)
{

    char *end;
    char *position;
    char value[MAX_ELEMENT_LENGTH];
    char *start = NULL;
    int length;
    
    if (pattern == NULL) {
        return -1;
    }
    
    while (*header == SPACE) header ++;
    end = position = header;

    for ( ; pattern->format != NULL;  pattern++) {            
        end = FindNextComponent(position, pattern);
        bzero(value, sizeof(value));
        start = SkipLeadingSeparator(header, position);

        length = end - start;
        if (length > 0 && length < MAX_ELEMENT_LENGTH) {         
            strncpy(value, start, length);
        }

        if (pattern->Validate != NULL) {
            if (pattern->Validate(value) == -1) {
                return -1;
            }
        }
        if (pattern->Parse != NULL) {
            pattern->Parse(value, target + pattern->offset);
        }

        position = end;
    }

    return 0;
}

#undef MAX_ELEMENT_LENGTH
