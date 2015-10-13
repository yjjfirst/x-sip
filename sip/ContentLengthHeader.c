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
    Copy2Target(c, "Content-Length", p);

    return c;
}

struct Header *ParseContentLength(char *string)
{
    struct ContentLengthHeader *c = CreateContentLengthHeader();
    Parse(string, c, GetContentLengthHeaderPattern());

    return (struct Header *)c;
}

char *ContentLengthGetName(struct ContentLengthHeader *c)
{
    return c->headerBase.name;
}

int ContentLengthGetLength(struct ContentLengthHeader *c)
{
    return c->length;
}

char *ContentLengthHeader2String(char *result, struct Header *c)
{
    return ToString(result, c, GetContentLengthHeaderPattern());
}
