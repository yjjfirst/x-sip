#include <string.h>
#include <stdio.h>

#include "RequestLine.h"

struct ParsePattern RequestLinePattern[] = {
    {"method",     SPACE, 0, OFFSETOF(struct RequestLine, Method)},
    {"uri",        SPACE, 0, OFFSETOF(struct RequestLine, Request_URI)},
    {"sip_version",CRLF,  1, OFFSETOF(struct RequestLine, SIP_Version)},
    {NULL, NULL, 0, 0}
};

struct ParsePattern *GetRequestLinePattern ()
{
    return RequestLinePattern;
}
