#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "RequestLine.h"
#include "Parser.h"
#include "Header.h"

#define METHOD_MAX_LENGTH 16
#define SIP_VERSION_LENGTH 8

struct RequestLine {
    char method[METHOD_MAX_LENGTH + 1];
    struct URI *requestUri;
    char sipVersion[SIP_VERSION_LENGTH];
};

int RequestLineMethodLegal(char *method)
{
    if (strcmp(method, "REGISTER") == 0 || strcmp(method, "INVITE") == 0)
        return TRUE;
    return FALSE;
}

int RequestLineSipVersionLegal(char *version)
{
    if (strcmp (version, "SIP/2.0") == 0)
        return TRUE;
    return FALSE;
}

struct HeaderPattern RequestLinePattern[] = {
    {"*", EMPTY, SPACE, 0, OFFSETOF(struct RequestLine, method), ParseStringElement, RequestLineMethodLegal, StringElement2String},
    {"*", SPACE, SPACE, 0, OFFSETOF(struct RequestLine, requestUri), ParseURI, NULL, Uri2String},
    {"*", SPACE, EMPTY, 0, OFFSETOF(struct RequestLine, sipVersion), ParseStringElement, NULL,StringElement2String},
    {NULL, 0, 0, 0, 0, 0}
};

struct HeaderPattern *GetRequestLinePattern ()
{
    return RequestLinePattern;
}

int ParseRequestLine(char *string, struct RequestLine *r)
{
    return Parse(string, r, GetRequestLinePattern());
}

char *RequestLineGetMethod (struct RequestLine *r)
{
    return r->method;
}

int RequestLineSetMethod (struct RequestLine *r, char *method)
{
    struct HeaderPattern *p = &RequestLinePattern[0];

    Copy2Target(r, method, p);        
    return 0;
}

char *RequestLineGetSipVersion(struct RequestLine *r)
{
    return r->sipVersion;
}

int RequestLineSetSipVersion(struct RequestLine *r, char *version)
{
    struct HeaderPattern *p = &RequestLinePattern[2];

    Copy2Target(r, version, p);        
    return 0;
}

int RequestLineSetUri(struct RequestLine *r, struct URI *u)
{
    DestoryUri(r->requestUri);
    r->requestUri = u;

    return 0;
}

struct URI *RequestLineGetUri(struct RequestLine *r)
{
    return r->requestUri;
}

char *RequestLine2String(char *string, struct RequestLine *r)
{
    return ToString(string, r, GetRequestLinePattern());
}

struct RequestLine *CreateRequestLine(SIP_METHOD m, struct URI *u)
{
    struct RequestLine *r = CreateEmptyRequestLine();
    RequestLineSetMethod(r, "REGISTER");
    RequestLineSetUri(r, u);
    RequestLineSetSipVersion(r, "SIP/2.0");

    return r;
}

struct RequestLine *CreateEmptyRequestLine()
{
    struct RequestLine *requestLine = calloc (1, sizeof(struct RequestLine));
    requestLine->requestUri = CreateEmptyUri();
    return requestLine;
}

void DestoryRequestLine(struct RequestLine *requestLine)
{
    if (requestLine != NULL) {
        DestoryUri(requestLine->requestUri);
        free(requestLine);
    }
}
