#include <stdlib.h>
#include "ViaHeader.h"
#include "Header.h"
#include "Parser.h"
#include "URI.h"

struct ViaHeader {
    struct Header headerBase;
    char transport[32];
    struct URI *uri;
    char parameters[128];
};

struct HeaderPattern ViaHeaderPattern []= {
    {"*", EMPTY, COLON, 0, OFFSETOF(struct ViaHeader, headerBase), ParseString, NULL, String2String},
    {"//", COLON, SPACE, 0, OFFSETOF(struct ViaHeader, transport), ParseString, NULL, String2String},
    {"*", SPACE, SEMICOLON, 1, OFFSETOF(struct ViaHeader, uri), ParseURI, NULL, Uri2String},
    {"*", SEMICOLON, EMPTY, 0, OFFSETOF(struct ViaHeader, parameters), ParseString, NULL, String2String},
    {NULL, 0, 0, 0, 0},
};

struct HeaderPattern *GetViaPattern()
{
    return ViaHeaderPattern;
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
        free(via->uri);
    via->uri = uri;
}

char *ViaHeaderGetParameters(struct ViaHeader *via)
{
    return via->parameters;
}

void ViaHeaderSetParameters(struct ViaHeader *via, char *parameters)
{
    struct HeaderPattern *p = &ViaHeaderPattern[3];
    Copy2Target(via, parameters, p);
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
    
    via->uri = CreateEmptyUri();
    return via;
}

struct ViaHeader *CreateViaHeader(struct URI *uri)
{
    struct ViaHeader *via = CreateEmptyViaHeader();
    
    ViaHeaderSetName(via, "Via");
    ViaHeaderSetTransport(via, "SIP/2.0/UDP");
    ViaHeaderSetUri(via, uri);
    
    return via;
}

void DestoryViaHeader(struct Header *via)
{
    struct ViaHeader *v = (struct ViaHeader *)via;
    if (v != NULL) {
        DestoryUri(v->uri);
        free(v);
    }
}
