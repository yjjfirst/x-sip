#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Header.h"
#include "Parser.h"
#include "ContactHeader.h"
#include "URI.h"

struct ContactHeader {
    struct Header headerBase;
    char displayName[32];
    struct URI *uri;
    char parameters[128];
};

struct HeaderPattern ContactName = {
};

struct HeaderPattern ContactHeaderPattern[] = {
    {"*",EMPTY, COLON, 0, OFFSETOF(struct ContactHeader, headerBase), ParseString, NULL, String2String },
    {"*",COLON, LEFT_ANGLE, 0,OFFSETOF(struct ContactHeader, displayName),ParseString, NULL, String2String },
    {"*",LEFT_ANGLE, RIGHT_ANGLE, 0, OFFSETOF(struct ContactHeader, uri), ParseURI, NULL, Uri2String },
    {"*",RIGHT_ANGLE, SEMICOLON, 0, 0, NULL, NULL, String2String },
    {"*",SEMICOLON, EMPTY, 0, OFFSETOF(struct ContactHeader, parameters),ParseString, NULL, String2String },
    {NULL}
};

struct HeaderPattern ContactHeaderWithQuotedDisplayNamePattern[] = {
    { "*", EMPTY, COLON, 0, OFFSETOF(struct ContactHeader, headerBase), ParseString, NULL, String2String},
    { "*", COLON, QUOTE, 0, 0, NULL, NULL, String2String},
    { "*", QUOTE, QUOTE, 0, OFFSETOF(struct ContactHeader, displayName), ParseString, NULL, String2String},
    { "*", QUOTE, LEFT_ANGLE, 0, OFFSETOF(struct ContactHeader,displayName), NULL, NULL, String2String},
    { "*", LEFT_ANGLE, RIGHT_ANGLE, 0, OFFSETOF(struct ContactHeader, uri), ParseURI, NULL, Uri2String},
    { "*", RIGHT_ANGLE, SEMICOLON, 0, 0, NULL, NULL, String2String},
    { "*", SEMICOLON, EMPTY, 0, OFFSETOF(struct ContactHeader, parameters), ParseString, NULL, String2String},
    {NULL}
};

struct HeaderPattern ContactHeaderNoDisplayNamePattern[] = {
    { "*",  EMPTY, COLON, 0, OFFSETOF(struct ContactHeader, headerBase), ParseString, NULL, String2String},
    { "*",  COLON, SEMICOLON, 1, OFFSETOF(struct ContactHeader, uri), ParseURI, NULL, Uri2String},
    { "*",  SEMICOLON, EMPTY, 0, OFFSETOF(struct ContactHeader, parameters),ParseString, NULL, String2String},
    {NULL}
};

struct HeaderPattern *GetContactHeaderPattern(char *header)
{  
    struct HeaderPattern *pattern = NULL;
    char *token;

    while (*header == SPACE) header ++;
    token = NextSeparator(header) + 1;
    while (*token == SPACE) token ++;

    if (strncmp(token, "sip:", 4) == 0) {
        pattern = ContactHeaderNoDisplayNamePattern;
        return pattern;
    }

    token = NextSeparator(token);
    if (*token == QUOTE)  {
        pattern = ContactHeaderWithQuotedDisplayNamePattern;
    }
    else {
        pattern = ContactHeaderPattern;
    }
    
    return pattern;
}
struct Header *ParseContactHeader(char *string)
{
    struct ContactHeader *header = CreateContactHeader();
    Parse(string, header, GetContactHeaderPattern(string));
    
    return (struct Header *)header;
}

void ContactHeaderSetName(struct ContactHeader *header, char *name)
{
    struct HeaderPattern *p = &ContactHeaderPattern[0];
    Copy2Target(header, name, p);
}

char *ContactHeaderGetName(struct ContactHeader *toHeader)
{
    return toHeader->headerBase.name;
}

char *ContactHeaderGetDisplayName(struct ContactHeader *toHeader)
{
    return toHeader->displayName;
}

void ContactHeaderSetDisplayName(struct ContactHeader *header, char *name)
{
    struct HeaderPattern *p = &ContactHeaderPattern[1];
    Copy2Target(header, name, p);
}

struct URI *ContactHeaderGetUri(struct ContactHeader *header)
{
    return header->uri;
}

void ContactHeaderSetUri(struct ContactHeader *header, struct URI *uri)
{
    if (header->uri != NULL)
        free(header->uri);
    header->uri = uri;
}

char *ContactHeaderGetParameters(struct ContactHeader *toHeader)
{
    return toHeader->parameters;
}

char *ContactHeader2String(char *result, struct Header *contact)
{
    return ToString(result, contact, ContactHeaderWithQuotedDisplayNamePattern);
}

struct ContactHeader *CreateContactHeader()
{
    struct ContactHeader *header = (struct ContactHeader *)calloc(1, sizeof(struct ContactHeader));
    
    header->uri = CreateEmptyUri();
    return header;
}

void DestoryContactHeader(struct Header *h)
{

    struct ContactHeader *header = (struct ContactHeader *)h;
    if (header != NULL) {
        DestoryUri(header->uri);
        free(header);
    }
}
