#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ToHeader.h"
#include "list.h"

struct ToHeader {
    char name[16];
    char displayName[32];
    char uri[128];
    char parameters[128];
};

struct ParsePattern ToHeaderPattern[] = {
    { "*", EMPTY, COLON, 0, OFFSETOF(struct ToHeader, name), ParseAtomElement},
    { "*", COLON, LEFT_ANGLE, 0, OFFSETOF(struct ToHeader, displayName), ParseAtomElement},
    { "*", LEFT_ANGLE, RIGHT_ANGLE, 0, OFFSETOF(struct ToHeader, uri), ParseAtomElement},
    { "*", RIGHT_ANGLE, SEMICOLON, 1, 0, ParseAtomElement},
    { "*", SEMICOLON, EMPTY, 0, OFFSETOF(struct ToHeader, parameters), ParseAtomElement},
    {NULL, 0, 0, 0}
};

struct ParsePattern ToHeaderWithQuotedDisplayNamePattern[] = {
    { "*", EMPTY, COLON, 0, OFFSETOF(struct ToHeader, name), ParseAtomElement},
    { "*", COLON, QUOTE, 1, 0, ParseAtomElement},
    { "*", QUOTE, QUOTE, 0, OFFSETOF(struct ToHeader, displayName), ParseAtomElement},
    { "*", QUOTE, LEFT_ANGLE, 1, 0, ParseAtomElement},
    { "*", LEFT_ANGLE, RIGHT_ANGLE, 0, OFFSETOF(struct ToHeader, uri), ParseAtomElement},
    { "*", RIGHT_ANGLE, SEMICOLON, 1, 0, ParseAtomElement},
    { "*", SEMICOLON, EMPTY, 0, OFFSETOF(struct ToHeader, parameters), ParseAtomElement},
    {NULL, 0, 0, 0}
};

struct ParsePattern ToHeaderNoDisplayNamePattern[] = {
    { "*",  EMPTY, COLON, 0, OFFSETOF(struct ToHeader, name), ParseAtomElement},
    { "*",  COLON, SEMICOLON, 0, OFFSETOF(struct ToHeader, uri), ParseAtomElement},
    { "*",  SEMICOLON, EMPTY, 0, OFFSETOF(struct ToHeader, parameters),ParseAtomElement},
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

char *ToHeaderGetName(struct ToHeader *toHeader)
{
    return toHeader->name;
}

char *ToHeaderGetDisplayName(struct ToHeader *toHeader)
{
    return toHeader->displayName;
}

char *ToHeaderGetUri(struct ToHeader *toHeader)
{
    return toHeader->uri;
}

char *ToHeaderGetParameters(struct ToHeader *toHeader)
{
    return toHeader->parameters;
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
    }
}
