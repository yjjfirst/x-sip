#include <string.h>
#include <stdio.h>

#include "RequestLine.h"

struct ParsePattern RequestLinePattern[] = {
    {"method",     EMPTY, SPACE, 0, OFFSETOF(struct RequestLine, Method)},
    {"uri",        SPACE, SPACE, 0, OFFSETOF(struct RequestLine, Request_URI)},
    {"sip_version",SPACE, EMPTY, 0, OFFSETOF(struct RequestLine, SIP_Version)},
    {NULL, 0, 0, 0}
};

struct ParsePattern *GetRequestLinePattern ()
{
    return RequestLinePattern;
}
