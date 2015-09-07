#include <stdlib.h>

#include "Header.h"
#include "MaxForwards.h"
#include "Parser.h"

struct MaxForwardsHeader {
    struct Header headerBase;
    int maxForwards;
};

struct ParsePattern MaxForwardsPattern[] = {
    {"*", EMPTY, COLON, 0, OFFSETOF(struct MaxForwardsHeader, headerBase), ParseStringElement},
    {"*", COLON, EMPTY, 0, OFFSETOF(struct MaxForwardsHeader, maxForwards), ParseIntegerElement},
    {NULL, 0, 0, 0, 0, 0},
};


struct MaxForwardsHeader *CreateMaxForwardsHeader()
{
    struct MaxForwardsHeader *m = (struct MaxForwardsHeader *) calloc(1, sizeof(struct MaxForwardsHeader));
    return m;
}

void DestoryMaxForwardsHeader(struct MaxForwardsHeader *m)
{
    if (m != NULL) {
        free(m);
    }
}

char *MaxForwardsGetName(struct MaxForwardsHeader *m)
{
    return m->headerBase.name;
}

int MaxForwardsGetMaxForwards(struct MaxForwardsHeader *m)
{
    return m->maxForwards;
}

struct ParsePattern *GetMaxForwardsPattern()
{
    return MaxForwardsPattern;
}
