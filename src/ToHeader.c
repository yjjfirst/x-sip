#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ToHeader.h"
#include "list.h"

struct ParsePattern ToHeaderPattern[] = {
    { "name",        EMPTY, COLON, 0, OFFSETOF(struct ToHeader, name), ParseAtomElement},
    { "displayName", COLON, LEFT_ANGLE, 0, OFFSETOF(struct ToHeader, displayName), ParseAtomElement},
    { "uri",         LEFT_ANGLE, RIGHT_ANGLE, 0, OFFSETOF(struct ToHeader, uri), ParseAtomElement},
    { "empty",       RIGHT_ANGLE, SEMICOLON, 1, 0, ParseAtomElement},
    { "parameters",  SEMICOLON, EMPTY, 0, OFFSETOF(struct ToHeader, parameters), ParseAtomElement},
    {NULL, 0, 0, 0}
};

struct ParsePattern ToHeaderWithQuotedDisplayNamePattern[] = {
    { "name",        EMPTY, COLON, 0, OFFSETOF(struct ToHeader, name), ParseAtomElement},
    { "empty",       COLON, QUOTE, 1, 0, ParseAtomElement},
    { "displayName", QUOTE, QUOTE, 0, OFFSETOF(struct ToHeader, displayName), ParseAtomElement},
    { "empty",       QUOTE, LEFT_ANGLE, 1, 0, ParseAtomElement},
    { "uri",         LEFT_ANGLE, RIGHT_ANGLE, 0, OFFSETOF(struct ToHeader, uri), ParseAtomElement},
    { "empty",       RIGHT_ANGLE, SEMICOLON, 1, 0, ParseAtomElement},
    { "parameters",  SEMICOLON, EMPTY, 0, OFFSETOF(struct ToHeader, parameters), ParseAtomElement},
    {NULL, 0, 0, 0}
};

struct ParsePattern ToHeaderNoDisplayNamePattern[] = {
    { "name",        EMPTY, COLON, 0, OFFSETOF(struct ToHeader, name), ParseAtomElement},
    { "uri",         COLON, SEMICOLON, 0, OFFSETOF(struct ToHeader, uri), ParseAtomElement},
    { "parameters",  SEMICOLON, EMPTY, 0, OFFSETOF(struct ToHeader, parameters),ParseAtomElement},
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
