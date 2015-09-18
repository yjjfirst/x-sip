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
    {"*", EMPTY, COLON, 0, OFFSETOF(struct CallIDHeader, headerBase), ParseStringElement, NULL, StringElement2String},
    {"*", COLON, EMPTY, 0, OFFSETOF(struct CallIDHeader, id), ParseStringElement, NULL, StringElement2String},
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

void CallIDHeader2String(char *result, struct Header *id)
{
    ToString(result, id, GetCallIDPattern());
}

struct HeaderPattern *GetCallIDPattern()
{
    return CallIdHeaderPattern;
}

