#include <stdlib.h>
#include "ViaHeader.h"
#include "Header.h"
#include "Parser.h"

struct ViaHeader {
    struct Header headerBase;
    char transport[32];
    char uri[128];
    char parameters[128];
};

struct HeaderPattern ViaHeaderPattern []= {
    {"*", EMPTY, COLON, 0, OFFSETOF(struct ViaHeader, headerBase), ParseStringElement, NULL, StringElement2String},
    {"//", COLON, SPACE, 0, OFFSETOF(struct ViaHeader, transport), ParseStringElement, NULL, StringElement2String},
    {"*", SPACE, SEMICOLON, 1, OFFSETOF(struct ViaHeader, uri), ParseStringElement, NULL, StringElement2String},
    {"*", SEMICOLON, EMPTY, 0, OFFSETOF(struct ViaHeader, parameters), ParseStringElement, NULL, StringElement2String},
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

char *ViaHeaderGetTransport(struct ViaHeader *via)
{
    return via->transport;
}

char *ViaHeaderGetUri(struct ViaHeader *via)
{
    return via->uri;
}

char *ViaHeaderGetParameters(struct ViaHeader *via)
{
    return via->parameters;
}

struct Header *ParseViaHeader(char *string) 
{
    struct ViaHeader *via =  CreateViaHeader();
    struct HeaderPattern *viaPattern = GetViaPattern();
    Parse(string, via, viaPattern);
    
    return (struct Header *)via; 
}

void ViaHeader2String(char *result, struct ViaHeader *via)
{
    ToString(result, via, GetViaPattern());
}

struct ViaHeader *CreateViaHeader()
{
    struct ViaHeader *via = (struct ViaHeader *)calloc(1, sizeof(struct ViaHeader));
    return via;
}

void DestoryViaHeader(struct Header *via)
{
    if (via != NULL) {
        free(via);
    }
}
