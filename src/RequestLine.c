#include <string.h>
#include <stdio.h>

#include "RequestLine.h"

int parseMethod (char *method, void *target) 
{
    struct RequestLine *rl = (struct RequestLine*) target;
    strcpy(rl->Method, method);
}

int parseURI(char *uri, void *target)
{
    struct RequestLine *rl = (struct RequestLine*) target;
    strcpy(rl->Request_URI, uri);
}

int parseSIPVersion(char *version, void *target)
{
    struct RequestLine *rl = (struct RequestLine*) target;
    strcpy(rl->SIP_Version, version);
}

struct ParsePattern RequestLinePattern[] = {
    {"method",     SPACE, 0, parseMethod},
    {"uri",        SPACE, 0, parseURI},
    {"sip_version",CRLF,  0, parseSIPVersion},
    {NULL, NULL, 0, NULL}
};

struct ParsePattern *GetRequestLinePattern ()
{
    return RequestLinePattern;
}
