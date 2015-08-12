#include "RequestLine.h"

#include <string.h>
#include <stdio.h>

#define SPACE " "
#define CRLF "\r\n"

struct ParseStruct {
    char *name;
    char *separator;
    int optional;
    int (*parser) (char *value, void *target);
};


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

struct ParseStruct RequestLineParser[] = {
    {"method",     SPACE, 0, parseMethod},
    {"uri",        SPACE, 0, parseURI},
    {"sip_version",CRLF,  0, parseSIPVersion},
    {NULL, NULL, 0, NULL}
};

int parseRequestLine(char *requestLineString, struct RequestLine *requestLine)
{
    char *curr = requestLineString;
    char *next = requestLineString;
    int i = 0;
    char value[128];

    for ( ; RequestLineParser[i].name != NULL; i ++) {
        if (!(curr = strstr(next, RequestLineParser[i].separator))) continue;
        bzero(value, sizeof(value));
        strncpy(value, next, curr - next);
        RequestLineParser[i].parser(value, (void *)requestLine);
        next = curr + strlen(RequestLineParser[i].separator);
    }

    return 0;
}
