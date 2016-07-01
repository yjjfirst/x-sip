#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "Header.h"
#include "ExpiresHeader.h"
#include "Parser.h"

struct ExpiresHeader 
{
    struct Header headerBase;
    int expires;
};

struct HeaderPattern ExpiresHeaderPattern[] = {
    {"*", EMPTY, COLON, 0, OFFSETOF(struct ExpiresHeader, headerBase), ParseString, NULL, String2String},
    {"*", COLON, EMPTY, 0, OFFSETOF(struct ExpiresHeader, expires), ParseInteger, NULL, Integer2String},
    {NULL, 0, 0, 0, 0, 0},
        };

struct HeaderPattern *GetExpiresPattern()
{
    return ExpiresHeaderPattern;
}

struct Header *ParseExpiresHeader(char *string)
{
    struct ExpiresHeader *e = CreateExpiresHeader(0);
    Parse(string, e, GetExpiresPattern());
    return (struct Header *)e;
}
   
void DestroyExpiresHeader (struct Header *header)
{
    free(header);
}

struct ExpiresHeader *CreateExpiresHeader (int expires) 
{ 
    struct ExpiresHeader *e = NULL;
    struct HeaderPattern *p = &ExpiresHeaderPattern[0];

    e = (struct ExpiresHeader *)calloc(1,sizeof (struct ExpiresHeader)); 
    Copy2Target(e, HEADER_NAME_EXPIRES, p);

    SetIntegerField((void *)e, expires, &ExpiresHeaderPattern[1]);

    return e;
}

char *ExpiresHeaderGetName(struct ExpiresHeader *e)
{
    return e->headerBase.name;
}

int ExpiresHeaderGetExpires(struct ExpiresHeader *e)
{
    assert(e != NULL);
    return e->expires;
}

void ExpiresHeaderSetExpires(struct ExpiresHeader *e, int expires)
{
    assert(e != NULL);
    SetIntegerField((void *)e, expires, &ExpiresHeaderPattern[1]);
}

char *ExpiresHeader2String(char *result, struct Header *e)
{
    return ToString(result, e, GetExpiresPattern());
}


