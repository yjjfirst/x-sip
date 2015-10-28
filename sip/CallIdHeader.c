#include <stdio.h>
#include <stdlib.h>

#include "Header.h"
#include "Parser.h"
#include "CallIdHeader.h"

struct CallIdHeader {
    struct Header headerBase;
    char id[64];
};

struct HeaderPattern CallIdHeaderPattern[] = {
    {"*", EMPTY, COLON, 0, OFFSETOF(struct CallIdHeader, headerBase), ParseString, NULL, String2String},
    {"*", COLON, EMPTY, 0, OFFSETOF(struct CallIdHeader, id), ParseString, NULL, String2String},
    {NULL, 0, 0, 0, 0, 0},
};

struct Header *ParseCallIdHeader(char *string)
{
    struct CallIdHeader *id = CreateCallIdHeader();
    Parse(string, id, GetCallIdPattern());

    return (struct Header *) id;
}

char *CallIdHeaderGetName(struct CallIdHeader *id)
{
    return id->headerBase.name;
}

void CallIdHeaderSetName(struct CallIdHeader *id)
{
    struct HeaderPattern *p = &CallIdHeaderPattern[0];
    
    Copy2Target(id, HEADER_NAME_CALLID, p);
}

char *CallIdHeaderGetID(struct CallIdHeader *id)
{
    return id->id;
}

void CallIdHeaderSetID(struct CallIdHeader *id)
{
    struct HeaderPattern *p = &CallIdHeaderPattern[1];
    Copy2Target(id, "1626200011", p);
}

char *CallIdHeader2String(char *result, struct Header *id)
{
    return ToString(result, id, GetCallIdPattern());
}

struct HeaderPattern *GetCallIdPattern()
{
    return CallIdHeaderPattern;
}

struct CallIdHeader *CreateCallIdHeader () 
{ 
    struct CallIdHeader *header = NULL;

    header = (struct CallIdHeader *)calloc(1,sizeof (struct CallIdHeader)); 
    CallIdHeaderSetName(header);
    CallIdHeaderSetID(header);
    return header;
}

DEFINE_DESTROYER(struct Header, DestoryCallIdHeader)
