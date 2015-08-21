#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ToHeader.h"
#include "list.h"

struct ParsePattern ToHeaderPattern[] = {
    { "name",        EMPTY, COLON, 0, OFFSETOF(struct ToHeader, name)},
    { "displayName", COLON, LEFT_ANGLE, 0, OFFSETOF(struct ToHeader, displayName)},
    { "uri",         LEFT_ANGLE, RIGHT_ANGLE, 0, OFFSETOF(struct ToHeader, uri)},
    { "empty",       RIGHT_ANGLE, SEMICOLON, 1, 0},
    { "parameters",  SEMICOLON, EMPTY, 0, OFFSETOF(struct ToHeader, parameters)},
    {NULL, 0, 0, 0}
};

struct ParsePattern ToHeaderWithQuotedDisplayNamePattern[] = {
    { "name",        EMPTY, COLON, 0, OFFSETOF(struct ToHeader, name)},
    { "empty",       COLON, QUOTE, 1, 0},
    { "displayName", QUOTE, QUOTE, 0, OFFSETOF(struct ToHeader, displayName)},
    { "empty",       QUOTE, LEFT_ANGLE, 1, 0},
    { "uri",         LEFT_ANGLE, RIGHT_ANGLE, 0, OFFSETOF(struct ToHeader, uri)},
    { "empty",       RIGHT_ANGLE, SEMICOLON, 1, 0},
    { "parameters",  SEMICOLON, EMPTY, 0, OFFSETOF(struct ToHeader, parameters)},
    {NULL, 0, 0, 0}
};

struct ParsePattern ToHeaderNoDisplayNamePattern[] = {
    { "name",        EMPTY, COLON, 0, OFFSETOF(struct ToHeader, name)},
    { "uri",         COLON, SEMICOLON, 0, OFFSETOF(struct ToHeader, uri)},
    { "parameters",  SEMICOLON, EMPTY, 0, OFFSETOF(struct ToHeader, parameters)},
    {NULL, 0, 0, 0}
};

struct ParsePattern *GetToHeaderPattern(char *header)
{  
    struct ParsePattern *pattern = NULL;
    char *token = NextToken(header);

    token ++;
    while (*token == SPACE) token ++;
    
    if (strncmp(token, "sip:", 4) == 0) {
        pattern = ToHeaderNoDisplayNamePattern;
        return pattern;
    }

    token = NextToken(token);
    if (*token == QUOTE)        
        pattern = ToHeaderWithQuotedDisplayNamePattern;
    else {
        pattern = ToHeaderPattern;
    }
    
    return pattern;
}


struct ToHeader *CreateToHeader()
{
    struct ToHeader *to = (struct ToHeader *)calloc(1, sizeof(struct ToHeader));
    return to;
}

void DestoryToHeader(struct ToHeader *to)
{
    if (to != NULL) {
        free(to);
        to = NULL;
    }
}
