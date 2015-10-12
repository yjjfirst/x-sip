#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Header.h"
#include "Parser.h"
#include "Contacts.h"
#include "URI.h"

struct ContactHeader {
    struct Header headerBase;
    char displayName[32];
    struct URI *uri;
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
        .parse = ParseString,
        .legal = NULL,
        .toString = String2String },
    { 
        .format ="*", 
        .startSeparator = COLON, 
        .endSeparator = LEFT_ANGLE,
        .mandatory = 0, 
        .offset = OFFSETOF(struct ContactHeader, displayName), 
        .parse = ParseString, 
        .legal = NULL, 
        .toString = String2String },
    { 
        "*", 
        LEFT_ANGLE, 
        RIGHT_ANGLE,
        0, 
        OFFSETOF(struct ContactHeader, uri), 
        ParseURI, 
        NULL, 
        Uri2String },
    { 
        "*", 
        RIGHT_ANGLE, 
        SEMICOLON, 
        0, 
        0, 
        NULL, 
        NULL, 
        String2String },
    { 
        "*", 
        SEMICOLON, 
        EMPTY, 
        0, 
        OFFSETOF(struct ContactHeader, parameters), 
        ParseString, 
        NULL, 
        String2String },
    {NULL}
};

struct HeaderPattern ContactsHeaderWithQuotedDisplayNamePattern[] = {
    { "*", EMPTY, COLON, 0, OFFSETOF(struct ContactHeader, headerBase), ParseString, NULL, String2String},
    { "*", COLON, QUOTE, 0, 0, NULL, NULL, String2String},
    { "*", QUOTE, QUOTE, 0, OFFSETOF(struct ContactHeader, displayName), ParseString, NULL, String2String},
    { "*", QUOTE, LEFT_ANGLE, 0, OFFSETOF(struct ContactHeader,displayName), NULL, NULL, String2String},
    { "*", LEFT_ANGLE, RIGHT_ANGLE, 0, OFFSETOF(struct ContactHeader, uri), ParseURI, NULL, Uri2String},
    { "*", RIGHT_ANGLE, SEMICOLON, 0, 0, NULL, NULL, String2String},
    { "*", SEMICOLON, EMPTY, 0, OFFSETOF(struct ContactHeader, parameters), ParseString, NULL, String2String},
    {NULL}
};

struct HeaderPattern ContactsHeaderNoDisplayNamePattern[] = {
    { "*",  EMPTY, COLON, 0, OFFSETOF(struct ContactHeader, headerBase), ParseString, NULL, String2String},
    { "*",  COLON, SEMICOLON, 1, OFFSETOF(struct ContactHeader, uri), ParseURI, NULL, Uri2String},
    { "*",  SEMICOLON, EMPTY, 0, OFFSETOF(struct ContactHeader, parameters),ParseString, NULL, String2String},
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

void ContactsHeaderSetName(struct ContactHeader *header, char *name)
{
    struct HeaderPattern *p = &ContactsHeaderPattern[0];
    Copy2Target(header, name, p);
}

char *ContactsHeaderGetName(struct ContactHeader *toHeader)
{
    return toHeader->headerBase.name;
}

char *ContactsHeaderGetDisplayName(struct ContactHeader *toHeader)
{
    return toHeader->displayName;
}

void ContactsHeaderSetDisplayName(struct ContactHeader *header, char *name)
{
    struct HeaderPattern *p = &ContactsHeaderPattern[1];
    Copy2Target(header, name, p);
}

struct URI *ContactsHeaderGetUri(struct ContactHeader *header)
{
    return header->uri;
}

void ContactsHeaderSetUri(struct ContactHeader *header, struct URI *uri)
{
    if (header->uri != NULL)
        free(header->uri);
    header->uri = uri;
}

char *ContactsHeaderGetParameters(struct ContactHeader *toHeader)
{
    return toHeader->parameters;
}

char *ContactsHeader2String(char *result, struct Header *contacts)
{
    return ToString(result, contacts, ContactsHeaderWithQuotedDisplayNamePattern);
}

struct ContactHeader *CreateContactsHeader()
{
    struct ContactHeader *header = (struct ContactHeader *)calloc(1, sizeof(struct ContactHeader));
    
    header->uri = CreateEmptyUri();
    return header;
}

void DestoryContactsHeader(struct Header *h)
{

    struct ContactHeader *header = (struct ContactHeader *)h;
    if (header != NULL) {
        DestoryUri(header->uri);
        free(header);
    }
}
