#include <stdio.h>
#include <stdlib.h>

#include "Header.h"
#include "Parser.h"
#include "CallIDHeader.h"

struct CallIDHeader {
    struct Header headerBase;
    char id[64];
};

struct HeaderPattern CallIdHeaderPattern[] = {
    {"*", EMPTY, COLON, 0, OFFSETOF(struct CallIDHeader, headerBase), ParseStringElement},
    {"*", COLON, EMPTY, 0, OFFSETOF(struct CallIDHeader, id), ParseStringElement},
    {NULL, 0, 0, 0, 0, 0},
};

DEFINE_CREATER(struct CallIDHeader, CreateCallIDHeader)
DEFINE_DESTROYER(struct Header, DestoryCallIDHeader)

struct Header *ParseCallIDHeader(char *string)
{
    struct CallIDHeader *id = CreateCallIDHeader();
    Parse(string, id, GetCallIDPattern());

    return (struct Header *) id;
}

char *CallIDHeaderGetName(struct CallIDHeader *id)
{
    return id->headerBase.name;
}

char *CallIDHeaderGetID(struct CallIDHeader *id)
{
    return id->id;
}

struct HeaderPattern *GetCallIDPattern()
{
    return CallIdHeaderPattern;
}

