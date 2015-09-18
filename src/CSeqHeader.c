#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Header.h"
#include "Parser.h"
#include "CSeqHeader.h"

struct CSeqHeader 
{
    struct Header headerBase;
    int seq;
    char method[16];
};

struct HeaderPattern CSeqHeaderPattern[] = {
    {"*", EMPTY, COLON, 0, OFFSETOF(struct CSeqHeader, headerBase),ParseStringElement, NULL, StringElement2String},
    {"*", COLON, SPACE, 0, OFFSETOF(struct CSeqHeader, seq), ParseIntegerElement, NULL, IntegerElement2String},
    {"*", SPACE, EMPTY, 0, OFFSETOF(struct CSeqHeader, method), ParseStringElement, NULL, StringElement2String},
    {NULL, 0, 0, 0, 0, 0},
};

struct HeaderPattern *GetCSeqHeaderPattern()
{
    return CSeqHeaderPattern;
}

DEFINE_CREATER(struct CSeqHeader, CreateCSeqHeader);
DEFINE_DESTROYER(struct Header, DestoryCSeqHeader);

struct Header *ParseCSeqHeader(char *string)
{
    struct CSeqHeader *c = CreateCSeqHeader();
    Parse(string, c, GetCSeqHeaderPattern());

    return (struct Header *)c;
}

char *CSeqHeaderGetName(struct CSeqHeader *c)
{
    return c->headerBase.name;
}
int CSeqHeaderGetSeq(struct CSeqHeader *c)
{
    return c->seq;
}
char *CSeqHeaderGetMethod(struct CSeqHeader *c)
{
    return c->method;
}

void CSeq2String(char *result, struct Header *c)
{
    ToString(result, c, GetCSeqHeaderPattern());
}