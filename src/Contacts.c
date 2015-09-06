#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Header.h"
#include "Contacts.h"

struct ToHeader {
    struct Header headerBase;
    char displayName[32];
    char uri[128];
    char parameters[128];
};

struct ParsePattern ContactsHeaderPattern[] = {
    { "*", EMPTY, COLON, 0, OFFSETOF(struct ToHeader, headerBase), ParseStringElement},
    { "*", COLON, LEFT_ANGLE, 0, OFFSETOF(struct ToHeader, displayName), ParseStringElement},
    { "*", LEFT_ANGLE, RIGHT_ANGLE, 0, OFFSETOF(struct ToHeader, uri), ParseStringElement},
    { "*", RIGHT_ANGLE, SEMICOLON, 1, 0, ParseStringElement},
    { "*", SEMICOLON, EMPTY, 0, OFFSETOF(struct ToHeader, parameters), ParseStringElement},
    {NULL, 0, 0, 0}
};

struct ParsePattern ContactsHeaderWithQuotedDisplayNamePattern[] = {
    { "*", EMPTY, COLON, 0, OFFSETOF(struct ToHeader, headerBase), ParseStringElement},
    { "*", COLON, QUOTE, 1, 0, ParseStringElement},
    { "*", QUOTE, QUOTE, 0, OFFSETOF(struct ToHeader, displayName), ParseStringElement},
    { "*", QUOTE, LEFT_ANGLE, 1, 0, ParseStringElement},
    { "*", LEFT_ANGLE, RIGHT_ANGLE, 0, OFFSETOF(struct ToHeader, uri), ParseStringElement},
    { "*", RIGHT_ANGLE, SEMICOLON, 1, 0, ParseStringElement},
    { "*", SEMICOLON, EMPTY, 0, OFFSETOF(struct ToHeader, parameters), ParseStringElement},
    {NULL, 0, 0, 0}
};

struct ParsePattern ContactsHeaderNoDisplayNamePattern[] = {
    { "*",  EMPTY, COLON, 0, OFFSETOF(struct ToHeader, headerBase), ParseStringElement},
    { "*",  COLON, SEMICOLON, 0, OFFSETOF(struct ToHeader, uri), ParseStringElement},
    { "*",  SEMICOLON, EMPTY, 0, OFFSETOF(struct ToHeader, parameters),ParseStringElement},
    {NULL, 0, 0, 0}
};

struct ParsePattern *GetContactsHeaderPattern(char *header)
{  
    struct ParsePattern *pattern = NULL;
    char *token = NextSeparator(header);

    token ++;
    while (*token == SPACE) token ++;
    
    if (strncmp(token, "sip:", 4) == 0) {
        pattern = ContactsHeaderNoDisplayNamePattern;
        return pattern;
    }

    token = NextSeparator(token);
    if (*token == QUOTE)        
        pattern = ContactsHeaderWithQuotedDisplayNamePattern;
    else {
        pattern = ContactsHeaderPattern;
    }
    
    return pattern;
}

char *ContactsHeaderGetName(struct ToHeader *toHeader)
{
    return toHeader->headerBase.name;
}

char *ContactsHeaderGetDisplayName(struct ToHeader *toHeader)
{
    return toHeader->displayName;
}

char *ContactsHeaderGetUri(struct ToHeader *toHeader)
{
    return toHeader->uri;
}

char *ContactsHeaderGetParameters(struct ToHeader *toHeader)
{
    return toHeader->parameters;
}

struct ToHeader *CreateContactsHeader()
{
    struct ToHeader *to = (struct ToHeader *)calloc(1, sizeof(struct ToHeader));
    return to;
}

void DestoryContactsHeader(struct ToHeader *to)
{
    if (to != NULL) {
        free(to);
    }
}
