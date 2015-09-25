#include <stdlib.h>

#include "Header.h"
#include "ExpiresHeader.h"

struct ExpiresHeader 
{
    struct Header headerBase;
    int expires;
};

struct HeaderPattern ExpiresHeaderPattern[] = {
    {"*", EMPTY, COLON, 0, OFFSETOF(struct ExpiresHeader, headerBase), ParseStringElement, NULL, StringElement2String},
    {"*", COLON, EMPTY, 0, OFFSETOF(struct ExpiresHeader, expires), ParseIntegerElement, NULL, IntegerElement2String},
    {NULL, 0, 0, 0, 0, 0},
};

struct HeaderPattern *GetExpiresPattern()
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

void ExpiresHeader2String(char *result, struct ExpiresHeader *e)
{
    ToString(result, e, GetExpiresPattern());
}
