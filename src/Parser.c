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

char *FindEndSeparator(char *header, struct HeaderPattern *pattern)
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

char *FindNextComponent(char *header, struct HeaderPattern *pattern)
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
int Parse(char *string, void* target, struct HeaderPattern *pattern)
{

    char *end;
    char *position;
    char value[MAX_ELEMENT_LENGTH];
    char *start = NULL;
    int length;
    
    if (pattern == NULL) {
        return -1;
    }
    
    while (*string == SPACE) string ++;
    end = position = string;

    for ( ; pattern->format != NULL;  pattern++) {            
        end = FindNextComponent(position, pattern);
        bzero(value, sizeof(value));
        start = SkipLeadingSeparator(string, position);

        length = end - start;
        if (length > 0 && length < MAX_ELEMENT_LENGTH) {         
            strncpy(value, start, length);
        }

        if (pattern->legal != NULL && pattern->legal(value) == 0) {
                return -1;
        }

        if (pattern->parse != NULL) {
            pattern->parse(value, target + pattern->offset);
        }

        position = end;
    }

    return 0;
}

#undef MAX_ELEMENT_LENGTH


char *StringElement2String(char *pos, void *element, struct HeaderPattern *p)
{
    if (p->startSeparator != EMPTY && strlen(element) != 0) {
        *pos = p->startSeparator;
        pos ++;
    }

    if (p->parse != NULL) {
        strcpy(pos, element);    
        return pos + strlen(element);
    } else {
        return pos;
    }
}

char *IntegerElement2String(char *pos, void *element, struct HeaderPattern *p)
{
    int *value = element;

    if (p->startSeparator != EMPTY) {
        *pos = p->startSeparator;
        pos ++;
    }
    
    if (p->parse != NULL) {
        pos = pos + sprintf(pos, "%d", *value);
    }
    return pos;
}

char *ToString (char *string, void *header, struct HeaderPattern *pattern)
{
    char *pos = string;
    char *element = NULL;
    struct HeaderPattern *p = pattern;

    for (; p->format != NULL; p++) {
        element = (char *)header + p->offset;        
        pos = p->toString(pos, element, p);
    }
    
    return pos;
}
