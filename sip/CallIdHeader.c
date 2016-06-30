#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "Header.h"
#include "Parser.h"
#include "CallIdHeader.h"
#include "StringExt.h"

struct CallIdHeader {
    struct Header headerBase;
    char id[CALLID_MAX_LENGTH];
};

struct HeaderPattern CallIdHeaderPattern[] = {
    {"*", EMPTY, COLON, 0, OFFSETOF(struct CallIdHeader, headerBase), ParseString, NULL, String2String},
    {"*", COLON, EMPTY, 0, OFFSETOF(struct CallIdHeader, id), ParseString, NULL, String2String},
    {NULL, 0, 0, 0, 0, 0},
};

struct Header *ParseCallIdHeader(char *string)
{
    struct CallIdHeader *id = CreateEmptyCallIdHeader();
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

char *CallIdHeaderGetId(struct CallIdHeader *id)
{
    assert (id != NULL);
    return id->id;
}

void CallIdHeaderSetID(struct CallIdHeader *id, char *idString)
{    
    struct HeaderPattern *p = &CallIdHeaderPattern[1];

    assert(idString != NULL);
    Copy2Target(id, idString, p);
}

BOOL CallIdHeaderMatched(struct CallIdHeader *id1, struct CallIdHeader *id2)
{
    assert (id1 != NULL);
    assert (id2 != NULL);

    return !StrcmpExt(id1->id, id2->id);
}

char *CallIdHeader2String(char *result, struct Header *id)
{
    return ToString(result, id, GetCallIdPattern());
}

struct HeaderPattern *GetCallIdPattern()
{
    return CallIdHeaderPattern;
}

char *GenerateCallIdString()
{
    return "97295390";
}

struct CallIdHeader *CreateEmptyCallIdHeader()
{
    return (struct CallIdHeader *)calloc(1,sizeof (struct CallIdHeader)); 
}

struct CallIdHeader *CreateCallIdHeader (char *idString) 
{ 
    struct CallIdHeader *header = CreateEmptyCallIdHeader();

    CallIdHeaderSetName(header);
    CallIdHeaderSetID(header, idString);
    return header;
}

struct CallIdHeader *CallIdHeaderDup(struct CallIdHeader *src)
{
    char *srcId = CallIdHeaderGetId(src);
    struct CallIdHeader *dest = CreateCallIdHeader(srcId);
    return dest;
}

DEFINE_DESTROYER(struct Header, DestroyCallIdHeader)
