#include <stdlib.h>

#include "Header.h"
#include "MaxForwardsHeader.h"
#include "Parser.h"

struct MaxForwardsHeader {
    struct Header headerBase;
    int maxForwards;
};

struct HeaderPattern MaxForwardsPattern[] = {
    {"*", EMPTY, COLON, 0, OFFSETOF(struct MaxForwardsHeader, headerBase), ParseString, NULL, String2String},
    {"*", COLON, EMPTY, 0, OFFSETOF(struct MaxForwardsHeader, maxForwards), ParseInteger, NULL, Integer2String},
    {NULL, 0, 0, 0, 0, 0},
};


struct Header *ParseMaxForwardsHeader(char *string)
{
    struct MaxForwardsHeader *maxForwards = CreateMaxForwardsHeader();
    struct HeaderPattern *maxForwardsPattern = GetMaxForwardsPattern();
    Parse(string, maxForwards, maxForwardsPattern);
    
    return (struct Header *)maxForwards;
}


char *MaxForwardsGetName(struct MaxForwardsHeader *m)
{
    return m->headerBase.name;
}

void MaxForwardsSetName(struct MaxForwardsHeader *m)
{
    struct HeaderPattern *p = &MaxForwardsPattern[0];

    Copy2Target(m, "Max-Forwards", p);
}

int MaxForwardsGetMaxForwards(struct MaxForwardsHeader *m)
{
    return m->maxForwards;
}

void MaxForwardsSetMaxForwards(struct MaxForwardsHeader *m)
{
    struct HeaderPattern *p = &MaxForwardsPattern[1];
    int maxForwards = 70;

    Copy2Target(m, (void *)&maxForwards, p);
}

struct HeaderPattern *GetMaxForwardsPattern()
{
    return MaxForwardsPattern;
}

char *MaxForwards2String(char *result, struct Header *m)
{
    return ToString(result, m, GetMaxForwardsPattern());
}

struct MaxForwardsHeader *CreateMaxForwardsHeader()
{
    struct MaxForwardsHeader *m = (struct MaxForwardsHeader *) calloc(1, sizeof(struct MaxForwardsHeader));
    MaxForwardsSetName(m);
    MaxForwardsSetMaxForwards(m);
    return m;
}

void DestoryMaxForwardsHeader(struct Header *m)
{
    if (m != NULL) {
        free(m);
    }
}
