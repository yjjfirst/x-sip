#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "ViaHeader.h"
#include "Header.h"
#include "Parser.h"
#include "URI.h"
#include "Parameter.h"
#include "StringExt.h"

#define TRANSPORT_PROTOCOL_NAME_MAX_LENGTH 32

struct ViaHeader {
    struct Header headerBase;
    char transport[TRANSPORT_PROTOCOL_NAME_MAX_LENGTH];
    struct URI *uri;
    struct Parameters *parameters;
};

struct HeaderPattern ViaHeaderPattern []= {
    {"*", EMPTY, COLON, 0, OFFSETOF(struct ViaHeader, headerBase), ParseString, NULL, String2String},
    {"//", COLON, SPACE, 0, OFFSETOF(struct ViaHeader, transport), ParseString, NULL, String2String},
    {"*", SPACE, SEMICOLON, 1, OFFSETOF(struct ViaHeader, uri), ParseUri, NULL, Uri2String},
    {"*", SEMICOLON, EMPTY, 0, OFFSETOF(struct ViaHeader, parameters), ParseParameters, NULL, Parameters2String},
    {NULL, 0, 0, 0, 0},
};

struct HeaderPattern *GetViaPattern()
{
    return ViaHeaderPattern;
}

BOOL ViaHeaderBranchMatched(struct ViaHeader *via1, struct ViaHeader *via2)
{
    assert(via1 != NULL && via2 != NULL);

    return !StrcmpExt(ViaHeaderGetParameter(via1, VIA_BRANCH_PARAMETER_NAME), 
                      ViaHeaderGetParameter(via2, VIA_BRANCH_PARAMETER_NAME));
}

BOOL ViaHeaderBranchMatchedByString(struct ViaHeader *via, char *string)
{
    assert(via != NULL && string != NULL);
    return !StrcmpExt(ViaHeaderGetParameter(via, VIA_BRANCH_PARAMETER_NAME), string);
}

BOOL ViaHeaderSendbyMatched(struct ViaHeader *via1, struct ViaHeader *via2)
{
    assert (via1 != NULL && via2 != NULL);

    char *sendBy1 = ViaHeaderGetParameter(via1, VIA_SENDBY_PARAMETER_NAME);
    char *sendBy2 = ViaHeaderGetParameter(via2, VIA_SENDBY_PARAMETER_NAME);
    
    return !StrcmpExt(sendBy1, sendBy2);
}

BOOL ViaHeaderMatched(struct ViaHeader *via1, struct ViaHeader *via2)
{
    assert(via1 != NULL);
    assert(via2 != NULL);
    
    if (!UriMatched(ViaHeaderGetUri(via1), ViaHeaderGetUri(via2))) {
        return FALSE;
    }

    if (StrcmpExt (ViaHeaderGetTransport(via1), ViaHeaderGetTransport(via2)) != 0) {
        return FALSE;
    }
    
    if (!ParametersMatched(ViaHeaderGetParameters(via1), ViaHeaderGetParameters(via2))) {
        return FALSE;
    }

    return TRUE;
}

char *ViaHeaderGetName(struct ViaHeader *via)
{
    return via->headerBase.name;
}

void ViaHeaderSetName(struct ViaHeader *via, char *name)
{
    struct HeaderPattern *p = &ViaHeaderPattern[0];
    Copy2Target(via, name, p);
}

char *ViaHeaderGetTransport(struct ViaHeader *via)
{
    return via->transport;
}

void ViaHeaderSetTransport(struct ViaHeader *via, char *t)
{
    struct HeaderPattern *p = &ViaHeaderPattern[1];
    Copy2Target(via, t, p);
}

struct URI *ViaHeaderGetUri(struct ViaHeader *via)
{
    return via->uri;
}

void ViaHeaderSetUri(struct ViaHeader *via, struct URI *uri)
{
    if (via->uri != NULL)
        DestroyUri(&via->uri);
    via->uri = uri;
}

char *ViaHeaderGetParameter(struct ViaHeader *via, char *name)
{
    return GetParameter(via->parameters, name);
}

struct Parameters *ViaHeaderGetParameters(struct ViaHeader *via)
{
    return via->parameters;
}

void ViaHeaderSetParameter(struct ViaHeader *via, char *name, char *value)
{
    AddParameter(via->parameters, name, value); 
}

void ViaHeaderSetParameters(struct ViaHeader *via, struct Parameters *parameters)
{
    if (via->parameters != NULL)
        DestroyParameters(via->parameters);
    via->parameters = parameters;
}

struct Header *ParseViaHeader(char *string) 
{
    struct ViaHeader *via =  CreateEmptyViaHeader();
    struct HeaderPattern *viaPattern = GetViaPattern();
    Parse(string, via, viaPattern);
    
    return (struct Header *)via; 
}

char *ViaHeader2String(char *result, struct Header *via)
{
    return ToString(result, via, GetViaPattern());
}

struct ViaHeader *CreateEmptyViaHeader()
{
    struct ViaHeader *via = (struct ViaHeader *)calloc(1, sizeof(struct ViaHeader));

    ViaHeaderSetName(via, "Via");
    via->uri = CreateEmptyUri();
    via->parameters = CreateParameters();
    return via;
}

struct ViaHeader *CreateViaHeader(struct URI *uri)
{
    struct ViaHeader *via = CreateEmptyViaHeader();
    
    ViaHeaderSetTransport(via, "SIP/2.0/UDP");
    ViaHeaderSetUri(via, uri);

    return via;
}

struct ViaHeader *ViaHeaderDup(struct ViaHeader *src)
{
    struct ViaHeader *dest = CreateEmptyViaHeader();
    
    DestroyUri(&dest->uri);
    DestroyParameters(dest->parameters);

    memcpy(dest, src, sizeof(struct ViaHeader));
    
    dest->uri = UriDup(src->uri);
    dest->parameters = ParametersDup(src->parameters);
    return dest;
}

void DestroyViaHeader(struct Header *via)
{
    struct ViaHeader *v = (struct ViaHeader *)via;
    if (v != NULL) {
        DestroyUri(&v->uri);
        DestroyParameters(v->parameters);
        free(v);
    }
}
