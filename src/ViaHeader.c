#include <stdlib.h>
#include "ViaHeader.h"

struct ViaHeader {
    char name[16];
    char transport[32];
    char uri[128];
    char parameters[128];
};

struct ParsePattern ViaHeaderPattern []= {
    {"*", EMPTY, COLON, 0, OFFSETOF(struct ViaHeader, name), ParseAtomElement},
    {"*", COLON, SPACE, 0, OFFSETOF(struct ViaHeader, transport), ParseAtomElement},
    {"*", SPACE, SEMICOLON, 0, OFFSETOF(struct ViaHeader, uri), ParseAtomElement},
    {"*", SEMICOLON, EMPTY, 0, OFFSETOF(struct ViaHeader, parameters), ParseAtomElement},
    {NULL, 0, 0, 0, 0},
};

struct ParsePattern *GetViaPattern()
{
    return ViaHeaderPattern;
}

char *ViaHeaderGetName(struct ViaHeader *via)
{
    return via->name;
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

struct ViaHeader *CreateViaHeader()
{
    struct ViaHeader *via = (struct ViaHeader *)calloc(1, sizeof(struct ViaHeader));
    return via;
}

void DestoryViaHeader(struct ViaHeader *via)
{
    if (via != NULL) {
        free(via);
    }
}
