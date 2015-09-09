#include <stdlib.h>

#include "Header.h"
#include "Parser.h"
#include "CSeqHeader.h"

struct CSeqHeader 
{
    struct Header headerBase;
    int seq;
    char method[16];
};

struct ParsePattern CSeqHeaderPattern[] = {
    {"*", EMPTY, COLON, 0, OFFSETOF(struct CSeqHeader, headerBase),ParseStringElement},
    {"*", COLON, SPACE, 0, OFFSETOF(struct CSeqHeader, seq), ParseIntegerElement},
    {"*", SPACE, EMPTY, 0, OFFSETOF(struct CSeqHeader, method), ParseStringElement},
    {NULL, 0, 0, 0, 0, 0},
};

struct ParsePattern *GetCSeqParsePattern()
{
    return CSeqHeaderPattern;
}

DEFINE_CREATER(struct CSeqHeader, CreateCSeqHeader);
DEFINE_DESTROYER(struct Header, DestoryCSeqHeader);

struct Header *ParseCSeqHeader(char *string)
{
    struct CSeqHeader *c = CreateCSeqHeader();
    Parse(string, c, GetCSeqParsePattern());

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
