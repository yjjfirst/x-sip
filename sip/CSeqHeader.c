#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "Header.h"
#include "Parser.h"
#include "CSeqHeader.h"
#include "SipMethod.h"
#include "StringExt.h"

struct CSeqHeader 
{
    struct Header headerBase;
    unsigned int seq;
    char method[METHOD_MAX_LENGTH];
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

DEFINE_DESTROYER(struct Header, DestroyCSeqHeader);

struct Header *ParseCSeqHeader(char *string)
{
    struct CSeqHeader *c = CreateCSeqHeader(0, SIP_METHOD_NAME_NONE);
    Parse(string, c, GetCSeqHeaderPattern());

    return (struct Header *)c;
}

char *CSeqHeaderGetName(struct CSeqHeader *c)
{
    return c->headerBase.name;
}

unsigned int CSeqHeaderGetSeq(struct CSeqHeader *c)
{
    return c->seq;
}

void CSeqHeaderSetSeq(struct CSeqHeader *c, int seq)
{
    SetIntegerField((void *)c , seq, &CSeqHeaderPattern[1]);
}

char *CSeqHeaderGetMethod(struct CSeqHeader *c)
{
    return c->method;
}

void CSeqHeaderSetMethod(struct CSeqHeader *c, char *method)
{
    Copy2Target(c, method, &CSeqHeaderPattern[2]);
}

char *CSeq2String(char *result, struct Header *c)
{
    return ToString(result, c, GetCSeqHeaderPattern());
}

static unsigned int CSeqGenerateSeqImpl()
{
    return 102;
}

unsigned int (*CSeqGenerateSeq)() = CSeqGenerateSeqImpl;

struct CSeqHeader *CreateCSeqHeader (int seq, char *method) 
{ 
    struct CSeqHeader *cseq = NULL;
    struct HeaderPattern *p = &CSeqHeaderPattern[0];    

    cseq = (struct CSeqHeader *)calloc(1,sizeof (struct CSeqHeader));
    Copy2Target(cseq, "CSeq", p);
    CSeqHeaderSetSeq(cseq, seq);
    CSeqHeaderSetMethod(cseq, method);

    return cseq;
}

struct CSeqHeader *CSeqHeaderDup(struct CSeqHeader *src)
{
    struct CSeqHeader *dest = CreateCSeqHeader(0,"");
    memcpy(dest, src, sizeof(struct CSeqHeader));

    return dest;
}

BOOL CSeqHeaderMethodMatched(struct CSeqHeader *c1, struct CSeqHeader *c2)
{
    assert(c1 != NULL && c2 != NULL);

    return !StrcmpExt (CSeqHeaderGetMethod(c1), CSeqHeaderGetMethod(c2));
}

BOOL CSeqMethodMatchedByName(struct CSeqHeader *c, char *string)
{
    assert(c != NULL && string != NULL);
    
    return !StrcmpExt(CSeqHeaderGetMethod(c), string);
}

BOOL CSeqHeadersMatched(struct CSeqHeader *c1, struct CSeqHeader *c2)
{
    assert (c1 != NULL);
    assert (c2 != NULL);

    if (!CSeqHeaderMethodMatched(c1, c2))
        return FALSE;

    return c1->seq == c2->seq;
}
