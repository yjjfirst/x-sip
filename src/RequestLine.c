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

static int ParseURI(char *header, void *target)
{
    struct HeaderPattern *pattern = GetURIHeaderPattern(header);
    struct URI **uri = target;

    Parse(header, *uri, pattern);
    
    return 0;
}

int RequestLineMethodLegal(char *method)
{
    if (strcmp(method, "REGISTER") == 0 || strcmp(method, "INVITE") == 0)
        return 1;
    return 0;
}

int RequestLineSipVersionLegal(char *version)
{
    if (strcmp (version, "SIP/2.0") == 0)
        return 1;
    return 0;
}

struct HeaderPattern RequestLinePattern[] = {
    {"*", EMPTY, SPACE, 0, OFFSETOF(struct RequestLine, method), ParseStringElement, RequestLineMethodLegal},
    {"*", SPACE, SPACE, 0, OFFSETOF(struct RequestLine, requestUri), ParseURI, NULL, Uri2String},
    {"*", SPACE, EMPTY, 0, OFFSETOF(struct RequestLine, sipVersion), ParseStringElement},
    {NULL, 0, 0, 0, 0, 0}
};

struct HeaderPattern *GetRequestLinePattern ()
{
    return RequestLinePattern;
}

char *RequestLineGetMethod (struct RequestLine *r)
{
    return r->method;
}

int RequestLineSetMethod (struct RequestLine *r, char *method)
{
    if (RequestLineMethodLegal(method) != 1)
        return -1;

    bzero(r->method, sizeof(r->method));
    strncpy(r->method, method, sizeof(r->method) - 1);
    return 0;
}

char *RequestLineGetSipVersion(struct RequestLine *r)
{
    return r->sipVersion;
}

int RequestLineSetSipVersion(struct RequestLine *r, char *version)
{
    if (!RequestLineSipVersionLegal(version))
        return -1;
    
    bzero(r->sipVersion, sizeof(r->sipVersion));
    strncpy(r->sipVersion, version, sizeof(r->sipVersion) - 1);
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

void RequestLine2String(struct RequestLine *r, char *string)
{
    char uri[HEADER_MAX_LENGTH] = {0};
    
    Uri2String(uri, r->requestUri);
    snprintf(string, HEADER_MAX_LENGTH, "%s %s %s", r->method, uri, r->sipVersion);
}

struct RequestLine *CreateRequestLine()
{
    struct RequestLine *requestLine = (struct RequestLine *)calloc (1, sizeof(struct RequestLine));
    requestLine->requestUri = CreateUri();
    return requestLine;
}

void DestoryRequestLine(struct RequestLine *requestLine)
{
    if (requestLine != NULL) {
        DestoryUri(requestLine->requestUri);
        free(requestLine);
    }
}
