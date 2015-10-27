#include <stdlib.h>
#include "ViaHeader.h"
#include "Header.h"
#include "Parser.h"
#include "URI.h"
#include "Parameter.h"

struct ViaHeader {
    struct Header headerBase;
    char transport[32];
    struct URI *uri;
    struct Parameters *parameters;
};

struct HeaderPattern ViaHeaderPattern []= {
    {"*", EMPTY, COLON, 0, OFFSETOF(struct ViaHeader, headerBase), ParseString, NULL, String2String},
    {"//", COLON, SPACE, 0, OFFSETOF(struct ViaHeader, transport), ParseString, NULL, String2String},
    {"*", SPACE, SEMICOLON, 1, OFFSETOF(struct ViaHeader, uri), ParseURI, NULL, Uri2String},
    {"*", SEMICOLON, EMPTY, 0, OFFSETOF(struct ViaHeader, parameters), ParseParameters, NULL, Parameters2String},
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
        DestoryUri(via->uri);
    via->uri = uri;
}

char *ViaHeaderGetParameter(struct ViaHeader *via, char *name)
{
    return GetParameter(via->parameters, name);
}

void ViaHeaderSetParameters(struct ViaHeader *via, struct Parameters *parameters)
{
    if (via->parameters != NULL)
        free(via->parameters);
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
    struct Parameters *p = CreateParameters();

    via->uri = CreateEmptyUri();
    via->parameters = p;
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
        DestoryParameters(v->parameters);
        free(v);
    }
}
