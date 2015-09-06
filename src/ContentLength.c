#include <stdlib.h>

#include "Header.h"
#include "ContentLength.h"

struct ContentLengthHeader 
{
    struct Header headerBase;
    int length;
};

struct ParsePattern ContentLengthParsePattern[] = {
    {"*", EMPTY, COLON, 0, OFFSETOF(struct ContentLengthHeader, headerBase), ParseStringElement},
    {"*", COLON, EMPTY, 0, OFFSETOF(struct ContentLengthHeader, length), ParseIntegerElement},
    {NULL, 0, 0, 0, 0, 0},
};

struct ParsePattern *GetContentLengthParsePattern()
{
    return ContentLengthParsePattern;
}

DEFINE_CREATER(struct ContentLengthHeader, CreateContentLengthHeader)
DEFINE_DESTROYER(struct ContentLengthHeader, DestoryContentLengthHeader)

char *ContentLengthGetName(struct ContentLengthHeader *c)
{
    return c->headerBase.name;
}

int ContentLengthGetLength(struct ContentLengthHeader *c)
{
    return c->length;
}
