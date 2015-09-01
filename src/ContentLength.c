#include <stdlib.h>
#include "ContentLength.h"

struct ContentLengthHeader 
{
    char name[16];
    int length;
};

struct ParsePattern ContentLengthParsePattern[] = {
    {"*", EMPTY, COLON, 0, OFFSETOF(struct ContentLengthHeader, name), ParseStringElement},
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
    return c->name;
}

int ContentLengthGetLength(struct ContentLengthHeader *c)
{
    return c->length;
}
