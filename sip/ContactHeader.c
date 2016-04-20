#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Header.h"
#include "Parser.h"
#include "ContactHeader.h"
#include "URI.h"
#include "Parameter.h"

struct ContactHeader {
    struct Header headerBase;
    char displayName[32];
    struct URI *uri;
    struct Parameters *parameters;
};

static struct HeaderPattern ContactNamePattern[] = {
    {"*",EMPTY, COLON, 0, OFFSETOF(struct ContactHeader, headerBase), ParseString, NULL, String2String }};
static struct HeaderPattern DisplayNamePattern[] = {
    {"*",COLON, LEFT_ANGLE, 0,OFFSETOF(struct ContactHeader, displayName),ParseString, NULL, String2String }};
static struct HeaderPattern UriPattern[] = {
    {"*",LEFT_ANGLE, RIGHT_ANGLE, 0, OFFSETOF(struct ContactHeader, uri), ParseUri, NULL, Uri2String }};
static struct HeaderPattern ParameterPattern[] = {
    {"*",SEMICOLON, EMPTY, 0, OFFSETOF(struct ContactHeader, parameters),ParseParameters, NULL, Parameters2String }};
static struct HeaderPattern PlaceholderUriParameter[] = {
    {"*^",RIGHT_ANGLE, SEMICOLON, 0, 0, NULL, NULL, String2String }};
static struct HeaderPattern PlaceholderNameDisplayname[] = {
    { "*^", COLON, QUOTE, 0, 0, NULL, NULL, String2String}};
struct HeaderPattern PlaceholderDisplaynameUri[] = {
    { "*^", QUOTE, LEFT_ANGLE, 0, OFFSETOF(struct ContactHeader,displayName), NULL, NULL, String2String}};
struct HeaderPattern ContactPatternFinal[8];

struct HeaderPattern *BuildQuotedDisplayNamePattern()
{
    int size = sizeof(struct HeaderPattern);
    
    bzero(ContactPatternFinal, sizeof(ContactPatternFinal));
    memcpy(&ContactPatternFinal[0], ContactNamePattern, size);
    memcpy(&ContactPatternFinal[1], PlaceholderNameDisplayname, size);

    memcpy(&ContactPatternFinal[2], DisplayNamePattern, size);
    ContactPatternFinal[2].startSeparator = QUOTE;
    ContactPatternFinal[2].endSeparator = QUOTE;

    memcpy(&ContactPatternFinal[3], PlaceholderDisplaynameUri, size);

    memcpy(&ContactPatternFinal[4], UriPattern, size);
    memcpy(&ContactPatternFinal[5], PlaceholderUriParameter, size);
    memcpy(&ContactPatternFinal[6], ParameterPattern, size);
    
    return ContactPatternFinal;

}

struct HeaderPattern *BuildContactPattern()
{
    int size = sizeof(struct HeaderPattern);
    
    bzero(ContactPatternFinal, sizeof(ContactPatternFinal));
    memcpy(&ContactPatternFinal[0], ContactNamePattern, size);
    memcpy(&ContactPatternFinal[1], DisplayNamePattern, size);
    memcpy(&ContactPatternFinal[2], UriPattern, size);
    memcpy(&ContactPatternFinal[3], PlaceholderUriParameter, size);
    memcpy(&ContactPatternFinal[4], ParameterPattern, size);

    return ContactPatternFinal;
}

struct HeaderPattern *BuildNoDisplayNamePattern()
{
    int size = sizeof (struct HeaderPattern);

    bzero(ContactPatternFinal, sizeof(ContactPatternFinal));

    memcpy(&ContactPatternFinal[0], ContactNamePattern, size);

    memcpy(&ContactPatternFinal[1], UriPattern, size);
    ContactPatternFinal[1].startSeparator = COLON;
    ContactPatternFinal[1].endSeparator = SEMICOLON;
    ContactPatternFinal[1].mandatory = 1;


    memcpy(&ContactPatternFinal[2], ParameterPattern, size);

    return ContactPatternFinal;
}

struct HeaderPattern *GetContactHeaderPattern(char *header)
{  
    struct HeaderPattern *pattern = NULL;
    char *token;

    while (*header == SPACE) header ++;
    token = NextSeparator(header) + 1;
    while (*token == SPACE) token ++;

    if (strncmp(token, URI_SCHEME_SIP, strlen(URI_SCHEME_SIP)) == 0) {
        pattern = BuildNoDisplayNamePattern();
        return pattern;
    }

    token = NextSeparator(token);
    if (*token == QUOTE)  {
        pattern = BuildQuotedDisplayNamePattern();
    }
    else {
        pattern = BuildContactPattern();
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
    struct HeaderPattern *p = ContactNamePattern;
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
    struct HeaderPattern *p = DisplayNamePattern;
    Copy2Target(header, name, p);
}

struct URI *ContactHeaderGetUri(struct ContactHeader *header)
{
    return header->uri;
}

void ContactHeaderSetUri(struct ContactHeader *header, struct URI *uri)
{
    if (header->uri != NULL)
        DestoryUri(header->uri);
    header->uri = uri;
}

char *ContactHeaderGetParameter(struct ContactHeader *toHeader, char *name)
{
    assert(toHeader != NULL);
    assert(name != NULL);

    return GetParameter(toHeader->parameters, name);
}

struct Parameters *ContactHeaderGetParameters(struct ContactHeader *header)
{
    assert(header != NULL);
    return header->parameters;
}

void ContactHeaderSetParameter(struct ContactHeader *header, char *name, char *value)
{
    AddParameter(header->parameters, name, value);
}

void ContactHeaderSetParameters(struct ContactHeader *header, struct Parameters *parameters)
{
    if (header->parameters != NULL)
        DestoryParameters(header->parameters);
    header->parameters = parameters;
}

void ContactHeaderRemoveParameters(struct ContactHeader *header)
{
    ClearParameters(header->parameters);
}

BOOL ContactHeaderMatched(struct ContactHeader *header1, struct ContactHeader *header2)
{
    assert(header1 != NULL);
    assert(header2 != NULL);

    if (!ParametersMatched(header1->parameters, header2->parameters))
        return FALSE;
    if (!UriMatched(header1->uri, header2->uri))
        return FALSE;

    return strcmp(header1->displayName, header2->displayName) == 0;
}

void GenerateTagImpl(char *tag)
{
    strncpy(tag, "1234567890", MAX_TAG_LENGTH-1);
}
void (*GenerateTag)(char *tag) = GenerateTagImpl;

struct ContactHeader *ContactHeaderDup(struct ContactHeader *src)
{
    struct ContactHeader *dest = CreateContactHeader();

    DestoryUri(dest->uri);
    DestoryParameters(dest->parameters);
    memcpy(dest, src, sizeof(struct ContactHeader));
    
    dest->uri = UriDup(src->uri);
    dest->parameters = ParametersDup(src->parameters);
    return dest;
}

char *ContactHeader2String(char *result, struct Header *contact)
{
    return ToString(result, contact, BuildQuotedDisplayNamePattern());
}

struct ContactHeader *CreateEmptyContactHeader()
{
    struct ContactHeader *header = (struct ContactHeader *)calloc(1, sizeof(struct ContactHeader));
    
    header->uri = CreateEmptyUri();
    header->parameters = CreateParameters();

    return header;
}

struct ContactHeader *CreateContactHeader()
{
    struct ContactHeader *c = CreateEmptyContactHeader();
    ContactHeaderSetName(c, HEADER_NAME_CONTACT);

    return c;
}

struct ContactHeader *CreateToHeader()
{
    struct ContactHeader *to = CreateEmptyContactHeader();
    ContactHeaderSetName(to, HEADER_NAME_TO);

    return to;
}

struct ContactHeader *CreateFromHeader()
{
    struct ContactHeader *from = CreateEmptyContactHeader();
    ContactHeaderSetName(from, HEADER_NAME_FROM);

    return from;
}

void DestoryContactHeader(struct Header *h)
{

    struct ContactHeader *header = (struct ContactHeader *)h;
    if (header != NULL) {
        DestoryParameters(header->parameters);
        DestoryUri(header->uri);
        free(header);
    }
}
