#include <stdio.h>
#include <stdlib.h>

#include "CallIDHeader.h"

struct CallIDHeader {
    char name[16];
    char id[64];
};

struct ParsePattern CallIdParsePattern[] = {
    {"*", EMPTY, COLON, 0, OFFSETOF(struct CallIDHeader, name), ParseStringElement},
    {"*", COLON, EMPTY, 0, OFFSETOF(struct CallIDHeader, id), ParseStringElement},
    {NULL, 0, 0, 0, 0, 0},
};

DEFINE_HEADER_CREATER(struct CallIDHeader, CreateCallIDHeader)
DEFINE_HEADER_DESTROYER(struct CallIDHeader, DestoryCallIDHeader)

char *CallIDHeaderGetName(struct CallIDHeader *id)
{
    return id->name;
}

char *CallIDHeaderGetID(struct CallIDHeader *id)
{
    return id->id;
}

struct ParsePattern *GetCallIDPattern()
{
    return CallIdParsePattern;
}

