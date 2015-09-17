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

struct HeaderPattern ContactName = {
};

struct HeaderPattern ContactsHeaderPattern[] = {
    {     
        .format = "*",
        .startSeparator = EMPTY,
        .endSeparator =  COLON,
        .mandatory = 0,
        .offset = OFFSETOF(struct ContactHeader, headerBase),
        .parse = ParseStringElement,
        .legal = NULL,
        .toString = StringElement2String },
    { 
        .format ="*", 
        .startSeparator = COLON, 
        .endSeparator = LEFT_ANGLE,
        .mandatory = 0, 
        .offset = OFFSETOF(struct ContactHeader, displayName), 
        .parse = ParseStringElement, 
        .legal = NULL, 
        .toString = StringElement2String },
    { 
        "*", 
        LEFT_ANGLE, 
        RIGHT_ANGLE,
        0, 
        OFFSETOF(struct ContactHeader, uri), 
        ParseStringElement, 
        NULL, 
        StringElement2String },
    { 
        "*", 
        RIGHT_ANGLE, 
        SEMICOLON, 
        0, 
        0, 
        NULL, 
        NULL, 
        StringElement2String },
    { 
        "*", 
        SEMICOLON, 
        EMPTY, 
        0, 
        OFFSETOF(struct ContactHeader, parameters), 
        ParseStringElement, 
        NULL, 
        StringElement2String },
    {NULL}
};

struct HeaderPattern ContactsHeaderWithQuotedDisplayNamePattern[] = {
    { "*", EMPTY, COLON, 0, OFFSETOF(struct ContactHeader, headerBase), ParseStringElement, NULL, StringElement2String},
    { "*", COLON, QUOTE, 0, 0, NULL, NULL, StringElement2String},
    { "*", QUOTE, QUOTE, 0, OFFSETOF(struct ContactHeader, displayName), ParseStringElement, NULL, StringElement2String},
    { "*", QUOTE, LEFT_ANGLE, 0, OFFSETOF(struct ContactHeader,displayName), NULL, NULL, StringElement2String},
    { "*", LEFT_ANGLE, RIGHT_ANGLE, 0, OFFSETOF(struct ContactHeader, uri), ParseStringElement, NULL, StringElement2String},
    { "*", RIGHT_ANGLE, SEMICOLON, 0, 0, NULL, NULL, StringElement2String},
    { "*", SEMICOLON, EMPTY, 0, OFFSETOF(struct ContactHeader, parameters), ParseStringElement, NULL, StringElement2String},
    {NULL}
};

struct HeaderPattern ContactsHeaderNoDisplayNamePattern[] = {
    { "*",  EMPTY, COLON, 0, OFFSETOF(struct ContactHeader, headerBase), ParseStringElement, NULL, StringElement2String},
    { "*",  COLON, SEMICOLON, 1, OFFSETOF(struct ContactHeader, uri), ParseStringElement, NULL, StringElement2String},
    { "*",  SEMICOLON, EMPTY, 0, OFFSETOF(struct ContactHeader, parameters),ParseStringElement, NULL, StringElement2String},
    {NULL}
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

void ContactsHeader2String(char *result, struct ContactHeader *contacts)
{
    ToString(result, contacts, ContactsHeaderWithQuotedDisplayNamePattern);
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
