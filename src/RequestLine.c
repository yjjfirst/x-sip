#include <string.h>
#include <stdio.h>

#include "RequestLine.h"

static int ParseURI(char *header, void *target)
{
    struct ParsePattern *pattern = GetURIParsePattern(header);
    Parse(header, target, pattern);
}

struct ParsePattern RequestLinePattern[] = {
    {"method",     EMPTY, SPACE, 0, OFFSETOF(struct RequestLine, Method), ParseAtomElement},
    {"uri",        SPACE, SPACE, 0, OFFSETOF(struct RequestLine, Request_URI), ParseURI},
    {"sip_version",SPACE, EMPTY, 0, OFFSETOF(struct RequestLine, SIP_Version), ParseAtomElement},
    {NULL, 0, 0, 0}
};

struct ParsePattern *GetRequestLinePattern ()
{
    return RequestLinePattern;
}
