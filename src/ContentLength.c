#include <stdlib.h>

#include "Header.h"
#include "Parser.h"
#include "ContentLength.h"

struct ContentLengthHeader 
{
    struct Header headerBase;
    int length;
};

struct HeaderPattern ContentLengthHeaderPattern[] = {
    {"*", EMPTY, COLON, 0, OFFSETOF(struct ContentLengthHeader, headerBase), ParseStringElement},
    {"*", COLON, EMPTY, 0, OFFSETOF(struct ContentLengthHeader, length), ParseIntegerElement},
    {NULL, 0, 0, 0, 0, 0},
};

struct HeaderPattern *GetContentLengthHeaderPattern()
{
    return ContentLengthHeaderPattern;
}

DEFINE_CREATER(struct ContentLengthHeader, CreateContentLengthHeader)
DEFINE_DESTROYER(struct Header, DestoryContentLengthHeader)

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
