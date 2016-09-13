#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Header.h"
#include "Parser.h"
#include "ContactHeader.h"
#include "URI.h"
#include "Parameter.h"
#include "StringExt.h"
#include "System.h"

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
    CONTACT_HEADER *header = CreateContactHeader();
    Parse(string, header, GetContactHeaderPattern(string));
    
    return (struct Header *)header;
}

void ContactHeaderSetName(CONTACT_HEADER *header, char *name)
{
    struct HeaderPattern *p = ContactNamePattern;
    Copy2Target(header, name, p);
}

char *ContactHeaderGetName(CONTACT_HEADER *toHeader)
{
    return toHeader->headerBase.name;
}

char *ContactHeaderGetDisplayName(CONTACT_HEADER *toHeader)
{
    return toHeader->displayName;
}

void ContactHeaderSetDisplayName(CONTACT_HEADER *header, char *name)
{
    struct HeaderPattern *p = DisplayNamePattern;
    Copy2Target(header, name, p);
}

struct URI *ContactHeaderGetUri(CONTACT_HEADER *header)
{
    assert(header != NULL);
    return header->uri;
}

void ContactHeaderSetUri(CONTACT_HEADER *header, struct URI *uri)
{
    if (header->uri != NULL)
        DestroyUri(&header->uri);
    header->uri = uri;
}

char *ContactHeaderGetParameter(CONTACT_HEADER *toHeader, char *name)
{
    assert(toHeader != NULL);
    assert(name != NULL);

    return GetParameter(toHeader->parameters, name);
}

struct Parameters *ContactHeaderGetParameters(CONTACT_HEADER *header)
{
    assert(header != NULL);
    return header->parameters;
}

void ContactHeaderSetParameter(CONTACT_HEADER *header, char *name, char *value)
{
    AddParameter(header->parameters, name, value);
}

void ContactHeaderSetParameters(CONTACT_HEADER *header, struct Parameters *parameters)
{
    if (header->parameters != NULL)
        DestroyParameters(&header->parameters);
    header->parameters = parameters;
}

void ContactHeaderRemoveParameters(CONTACT_HEADER *header)
{
    ClearParameters(header->parameters);
}

BOOL ContactHeaderMatched(CONTACT_HEADER *header1, CONTACT_HEADER *header2)
{
    assert(header1 != NULL);
    assert(header2 != NULL);

    if (!ParametersMatched(header1->parameters, header2->parameters))
        return FALSE;
    if (!UriMatched(header1->uri, header2->uri))
        return FALSE;

    return StrcmpExt(header1->displayName, header2->displayName) == 0;
}

void GenerateTagImpl(char *tag)
{
    srand(GetSeed());
    for (int i = 0 ; i < MAX_TAG_LENGTH; i ++) {
        tag[i] = 48 + rand() % 10;
    }

    tag[MAX_TAG_LENGTH] = 0;
}

void (*GenerateTag)(char *tag) = GenerateTagImpl;

CONTACT_HEADER *ContactHeaderDup(CONTACT_HEADER *src)
{
    CONTACT_HEADER *dest = CreateContactHeader();

    DestroyUri(&dest->uri);
    DestroyParameters(&dest->parameters);
    memcpy(dest, src, sizeof(struct ContactHeader));
    
    dest->uri = UriDup(src->uri);
    dest->parameters = ParametersDup(src->parameters);
    return dest;
}

char *ContactHeader2String(char *result, struct Header *contact)
{
    return ToString(result, contact, BuildQuotedDisplayNamePattern());
}

CONTACT_HEADER *CreateEmptyContactHeader()
{
    CONTACT_HEADER *header = (CONTACT_HEADER *)calloc(1, sizeof(struct ContactHeader));
    
    header->uri = CreateEmptyUri();
    header->parameters = CreateParameters();

    return header;
}

CONTACT_HEADER *CreateContactHeader()
{
    CONTACT_HEADER *c = CreateEmptyContactHeader();
    ContactHeaderSetName(c, HEADER_NAME_CONTACT);

    return c;
}

CONTACT_HEADER *CreateToHeader()
{
    CONTACT_HEADER *to = CreateEmptyContactHeader();
    ContactHeaderSetName(to, HEADER_NAME_TO);

    return to;
}

CONTACT_HEADER *CreateFromHeader()
{
    CONTACT_HEADER *from = CreateEmptyContactHeader();
    ContactHeaderSetName(from, HEADER_NAME_FROM);

    return from;
}

void DestroyContactHeader(struct Header *h)
{

    CONTACT_HEADER *header = (CONTACT_HEADER *)h;
    if (header != NULL) {
        DestroyParameters(&header->parameters);
        DestroyUri(&header->uri);
        free(header);
    }
}
