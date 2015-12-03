#include <stdlib.h>

#include "Header.h"
#include "Parser.h"
#include "ContentLengthHeader.h"

struct ContentLengthHeader 
{
    struct Header headerBase;
    int length;
};

struct HeaderPattern ContentLengthHeaderPattern[] = {
    {"*", EMPTY, COLON, 1, OFFSETOF(struct ContentLengthHeader, headerBase), ParseString, NULL, String2String},
    {"*", COLON, EMPTY, 1, OFFSETOF(struct ContentLengthHeader, length), ParseInteger, NULL, Integer2String},
    {NULL, 0, 0, 0, 0, 0},
};

struct HeaderPattern *GetContentLengthHeaderPattern()
{
    return ContentLengthHeaderPattern;
}

DEFINE_DESTROYER(struct Header, DestoryContentLengthHeader)

struct ContentLengthHeader *CreateContentLengthHeader ()
{ 
    struct ContentLengthHeader *c = NULL;
    struct HeaderPattern *p = &ContentLengthHeaderPattern[0];

    c = (struct ContentLengthHeader *)calloc(1,sizeof (struct ContentLengthHeader));
    Copy2Target(c, HEADER_NAME_CONTENT_LENGTH, p);

    return c;
}

struct Header *ParseContentLengthHeader(char *string)
{
    struct ContentLengthHeader *c = CreateContentLengthHeader();
    Parse(string, c, GetContentLengthHeaderPattern());

    return (struct Header *)c;
}

char *ContentLengthHeaderGetName(struct ContentLengthHeader *c)
{
    return c->headerBase.name;
}

void ContentLengthHeaderSetLength(struct ContentLengthHeader *c, int length)
{
    struct HeaderPattern *p = &ContentLengthHeaderPattern[1];
    
    SetIntegerField((void *)c, length, p);
}

int ContentLengthHeaderGetLength(struct ContentLengthHeader *c)
{
    return c->length;
}

char *ContentLengthHeader2String(char *result, struct Header *c)
{
    return ToString(result, c, GetContentLengthHeaderPattern());
}
