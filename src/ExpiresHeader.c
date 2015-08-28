#include <stdlib.h>
#include "ExpiresHeader.h"

struct ExpiresHeader 
{
    char name[16];
    int expires;
};

struct ParsePattern ExpiresHeaderPattern[] = {
    {"*", EMPTY, COLON, 0, OFFSETOF(struct ExpiresHeader, name), ParseStringElement},
    {"*", COLON, EMPTY, 0, OFFSETOF(struct ExpiresHeader, expires), ParseIntegerElement},
    {NULL, 0, 0, 0, 0, 0},
};

struct ParsePattern *GetExpiresPattern()
{
    return ExpiresHeaderPattern;
}
   
DEFINE_HEADER_CREATER(struct ExpiresHeader, CreateExpiresHeader)
DEFINE_HEADER_DESTROYER(struct ExpiresHeader, DestoryExpiresHeader)

char *ExpiresHeaderGetName(struct ExpiresHeader *e)
{
    return e->name;
}

int ExpiresHeaderGetExpires(struct ExpiresHeader *e)
{
    return e->expires;
}
