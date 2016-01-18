#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "Bool.h"
#include "RequestLine.h"
#include "Parser.h"
#include "Header.h"
#include "URI.h"

#define SIP_VERSION_LENGTH 16

struct RequestLine {
    char method[METHOD_MAX_LENGTH + 1];
    struct URI *requestUri;
    char sipVersion[SIP_VERSION_LENGTH];
};

BOOL RequestLineMethodLegal(char *method)
{
    if (strcmp(method, SIP_METHOD_NAME_REGISTER) == 0 
        || strcmp(method, SIP_METHOD_NAME_INVITE) == 0
        || strcmp(method, SIP_METHOD_NAME_ACK) == 0)
        return TRUE;
    return FALSE;
}

BOOL RequestLineSipVersionLegal(char *version)
{
    if (strcmp (version, "SIP/2.0") == 0)
        return TRUE;
    return FALSE;
}

struct HeaderPattern RequestLinePattern[] = {
    {"*", EMPTY, SPACE, 0, OFFSETOF(struct RequestLine, method), ParseString, RequestLineMethodLegal, String2String},
    {"*", SPACE, SPACE, 0, OFFSETOF(struct RequestLine, requestUri), ParseURI, NULL, Uri2String},
    {"*", SPACE, EMPTY, 0, OFFSETOF(struct RequestLine, sipVersion), ParseString, NULL,String2String},
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

char *RequestLineGetMethodName (struct RequestLine *r)
{
    return r->method;
}

SIP_METHOD RequestLineGetMethod(struct RequestLine *r)
{    
    return StringMap2MethodNumber(RequestLineGetMethodName(r));
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
    assert(r != NULL);
    assert(u != NULL);
    
    if (r->requestUri != NULL)
        DestoryUri(r->requestUri);
    r->requestUri = u;

    return 0;
}

struct URI *RequestLineGetUri(struct RequestLine *r)
{
    assert (r != NULL);
    return r->requestUri;
}

BOOL RequestLineMatched(struct RequestLine *r1, struct RequestLine *r2)
{
    assert(r1 != NULL);
    assert(r2 != NULL);

    if (!UriMatched(RequestLineGetUri(r1), RequestLineGetUri(r2))) {
        return FALSE;
    }

    if (strcmp(r1->method, r2->method) != 0) {
        return FALSE;
    }

    if (strcmp(r1->sipVersion, r2->sipVersion) != 0){
        return FALSE;
    }
    return TRUE;
}

char *RequestLine2String(char *string, struct RequestLine *r)
{
    assert(r != NULL);
    assert(string != NULL);
    return ToString(string, r, GetRequestLinePattern());
}

struct RequestLine *CreateRequestLine(SIP_METHOD m, struct URI *u)
{
    assert (u != NULL);

    struct RequestLine *r = CreateEmptyRequestLine();
    RequestLineSetMethod(r, MethodMap2String(m));
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
