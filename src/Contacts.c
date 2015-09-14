#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Header.h"
#include "Parser.h"
#include "Contacts.h"

struct ContactHeader {
    struct Header headerBase;
    char displayName[32];
    char uri[128];
    char parameters[128];
};

struct HeaderPattern ContactsHeaderPattern[] = {
    { "*", EMPTY, COLON, 0, OFFSETOF(struct ContactHeader, headerBase), ParseStringElement},
    { "*", COLON, LEFT_ANGLE, 0, OFFSETOF(struct ContactHeader, displayName), ParseStringElement},
    { "*", LEFT_ANGLE, RIGHT_ANGLE, 0, OFFSETOF(struct ContactHeader, uri), ParseStringElement},
    { "*", RIGHT_ANGLE, SEMICOLON, 0, 0, NULL},
    { "*", SEMICOLON, EMPTY, 0, OFFSETOF(struct ContactHeader, parameters), ParseStringElement},
    {NULL, 0, 0, 0}
};

struct HeaderPattern ContactsHeaderWithQuotedDisplayNamePattern[] = {
    { "*", EMPTY, COLON, 0, OFFSETOF(struct ContactHeader, headerBase), ParseStringElement},
    { "*", COLON, QUOTE, 0, 0, NULL},
    { "*", QUOTE, QUOTE, 0, OFFSETOF(struct ContactHeader, displayName), ParseStringElement},
    { "*", QUOTE, LEFT_ANGLE, 0, 0, NULL},
    { "*", LEFT_ANGLE, RIGHT_ANGLE, 0, OFFSETOF(struct ContactHeader, uri), ParseStringElement},
    { "*", RIGHT_ANGLE, SEMICOLON, 0, 0, NULL},
    { "*", SEMICOLON, EMPTY, 0, OFFSETOF(struct ContactHeader, parameters), ParseStringElement},
    {NULL, 0, 0, 0}
};

struct HeaderPattern ContactsHeaderNoDisplayNamePattern[] = {
    { "*",  EMPTY, COLON, 0, OFFSETOF(struct ContactHeader, headerBase), ParseStringElement},
    { "*",  COLON, SEMICOLON, 1, OFFSETOF(struct ContactHeader, uri), ParseStringElement},
    { "*",  SEMICOLON, EMPTY, 0, OFFSETOF(struct ContactHeader, parameters),ParseStringElement},
    {NULL, 0, 0, 0}
};

struct HeaderPattern *GetContactsHeaderPattern(char *header)
{  
    struct HeaderPattern *pattern = NULL;
    char *token;

    while (*header == SPACE) header ++;
    token = NextSeparator(header) + 1;
    while (*token == SPACE) token ++;
    

    if (strncmp(token, "sip:", 4) == 0) {
        pattern = ContactsHeaderNoDisplayNamePattern;
        return pattern;
    }

    token = NextSeparator(token);
    if (*token == QUOTE)  {      
        pattern = ContactsHeaderWithQuotedDisplayNamePattern;
    }
    else {
        pattern = ContactsHeaderPattern;
    }
    
    return pattern;
}
struct Header *ParseContactsHeader(char *string)
{
    struct ContactHeader *header = CreateContactsHeader();
    Parse(string, header, GetContactsHeaderPattern(string));
    
    return (struct Header *)header;
}

char *ContactsHeaderGetName(struct ContactHeader *toHeader)
{
    return toHeader->headerBase.name;
}

char *ContactsHeaderGetDisplayName(struct ContactHeader *toHeader)
{
    return toHeader->displayName;
}

char *ContactsHeaderGetUri(struct ContactHeader *toHeader)
{
    return toHeader->uri;
}

char *ContactsHeaderGetParameters(struct ContactHeader *toHeader)
{
    return toHeader->parameters;
}

struct ContactHeader *CreateContactsHeader()
{
    struct ContactHeader *to = (struct ContactHeader *)calloc(1, sizeof(struct ContactHeader));
    return to;
}

void DestoryContactsHeader(struct Header *to)
{
    if (to != NULL) {
        free(to);
    }
}
