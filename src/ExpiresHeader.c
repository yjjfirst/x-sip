#include <stdlib.h>

#include "Header.h"
#include "ExpiresHeader.h"

struct ExpiresHeader 
{
    struct Header headerBase;
    int expires;
};

struct ParsePattern ExpiresHeaderPattern[] = {
    {"*", EMPTY, COLON, 0, OFFSETOF(struct ExpiresHeader, headerBase), ParseStringElement},
    {"*", COLON, EMPTY, 0, OFFSETOF(struct ExpiresHeader, expires), ParseIntegerElement},
    {NULL, 0, 0, 0, 0, 0},
};

struct ParsePattern *GetExpiresPattern()
{
    return ExpiresHeaderPattern;
}
   
DEFINE_CREATER(struct ExpiresHeader, CreateExpiresHeader)
DEFINE_DESTROYER(struct ExpiresHeader, DestoryExpiresHeader)

char *ExpiresHeaderGetName(struct ExpiresHeader *e)
{
    return e->headerBase.name;
}

int ExpiresHeaderGetExpires(struct ExpiresHeader *e)
{
    return e->expires;
}
