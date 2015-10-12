#include <stdlib.h>

#include "Header.h"
#include "ExpiresHeader.h"
#include "Parser.h"

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

struct Header *ParseExpiresHeader(char *string)
{
    return NULL;
}
   
void DestoryExpiresHeader (struct Header *header)
{
    free(header);
}

struct ExpiresHeader *CreateExpiresHeader () 
{ 
    struct ExpiresHeader *e = NULL;
    struct HeaderPattern *p = &ExpiresHeaderPattern[0];

    e = (struct ExpiresHeader *)calloc(1,sizeof (struct ExpiresHeader)); 
    Copy2Target(e, "Expires", p);

    return e;
}

char *ExpiresHeaderGetName(struct ExpiresHeader *e)
{
    return e->headerBase.name;
}

int ExpiresHeaderGetExpires(struct ExpiresHeader *e)
{
    return e->expires;
}

char *ExpiresHeader2String(char *result, struct Header *e)
{
    return ToString(result, e, GetExpiresPattern());
}
