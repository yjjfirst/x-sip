#include <stdlib.h>

#include "Header.h"
#include "MaxForwards.h"
#include "Parser.h"

struct MaxForwardsHeader {
    struct Header headerBase;
    int maxForwards;
};

struct HeaderPattern MaxForwardsPattern[] = {
    {"*", EMPTY, COLON, 0, OFFSETOF(struct MaxForwardsHeader, headerBase), ParseStringElement, NULL, StringElement2String},
    {"*", COLON, EMPTY, 0, OFFSETOF(struct MaxForwardsHeader, maxForwards), ParseIntegerElement, NULL, IntegerElement2String},
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

int MaxForwardsGetMaxForwards(struct MaxForwardsHeader *m)
{
    return m->maxForwards;
}

struct HeaderPattern *GetMaxForwardsPattern()
{
    return MaxForwardsPattern;
}

void MaxForwards2String(char *result, struct Header *m)
{
    ToString(result, m, GetMaxForwardsPattern());
}

struct MaxForwardsHeader *CreateMaxForwardsHeader()
{
    struct MaxForwardsHeader *m = (struct MaxForwardsHeader *) calloc(1, sizeof(struct MaxForwardsHeader));
    return m;
}

void DestoryMaxForwardsHeader(struct Header *m)
{
    if (m != NULL) {
        free(m);
    }
}
