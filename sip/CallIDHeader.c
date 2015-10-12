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
    {"*", EMPTY, COLON, 0, OFFSETOF(struct CallIDHeader, headerBase), ParseString, NULL, String2String},
    {"*", COLON, EMPTY, 0, OFFSETOF(struct CallIDHeader, id), ParseString, NULL, String2String},
    {NULL, 0, 0, 0, 0, 0},
};

DEFINE_DESTROYER(struct Header, DestoryCallIDHeader)

struct CallIDHeader *CreateCallIDHeader () 
{ 
    struct CallIDHeader *header = NULL;
    struct HeaderPattern *p = &CallIdHeaderPattern[0];

    header = (struct CallIDHeader *)calloc(1,sizeof (struct CallIDHeader)); 
    Copy2Target(header, "Call-ID", p);
    
    return header;
}

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

void CallIDHeaderSetName(struct CallIDHeader *id)
{
    struct HeaderPattern *p = &CallIdHeaderPattern[0];
    
    Copy2Target(id, "Call-ID", p);
}

char *CallIDHeaderGetID(struct CallIDHeader *id)
{
    return id->id;
}

void CallIDHeaderSetID(struct CallIDHeader *id)
{
    struct HeaderPattern *p = &CallIdHeaderPattern[1];
    Copy2Target(id, "1234567890", p);
}

char *CallIDHeader2String(char *result, struct Header *id)
{
    return ToString(result, id, GetCallIDPattern());
}

struct HeaderPattern *GetCallIDPattern()
{
    return CallIdHeaderPattern;
}

