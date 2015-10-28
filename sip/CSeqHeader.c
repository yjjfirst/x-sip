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
    {"*", EMPTY, COLON, 0, OFFSETOF(struct CSeqHeader, headerBase),ParseString, NULL, String2String},
    {"*", COLON, SPACE, 0, OFFSETOF(struct CSeqHeader, seq), ParseInteger, NULL, Integer2String},
    {"*", SPACE, EMPTY, 0, OFFSETOF(struct CSeqHeader, method), ParseString, NULL, String2String},
    {NULL},
};

struct HeaderPattern *GetCSeqHeaderPattern()
{
    return CSeqHeaderPattern;
}

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

void CSeqHeaderSetSeq(struct CSeqHeader *c)
{
    int seq = 1;

    SetIntegerField((void *)c , seq, &CSeqHeaderPattern[1]);
}

char *CSeqHeaderGetMethod(struct CSeqHeader *c)
{
    return c->method;
}

void CSeqHeaderSetMethod(struct CSeqHeader *c)
{
    Copy2Target(c, "REGISTER", &CSeqHeaderPattern[2]);
}

char *CSeq2String(char *result, struct Header *c)
{
    return ToString(result, c, GetCSeqHeaderPattern());
}

struct CSeqHeader *CreateCSeqHeader () 
{ 
    struct CSeqHeader *cseq = NULL;
    struct HeaderPattern *p = &CSeqHeaderPattern[0];    

    cseq = (struct CSeqHeader *)calloc(1,sizeof (struct CSeqHeader));
    Copy2Target(cseq, "CSeq", p);
    CSeqHeaderSetSeq(cseq);
    CSeqHeaderSetMethod(cseq);

    return cseq;
}
