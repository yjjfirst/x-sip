#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "Bool.h"
#include "RequestLine.h"
#include "Parser.h"
#include "Header.h"
#include "URI.h"
#include "StringExt.h"

#define SIP_VERSION_LENGTH 16

struct RequestLine {
    char method[METHOD_MAX_LENGTH + 1];
    URI *requestUri;
    char sipVersion[SIP_VERSION_LENGTH];
};

BOOL RequestLineSipVersionLegal(char *version)
{
    if (StrcmpExt (version, SIP_VERSION) == 0)
        return TRUE;
    return FALSE;
}

struct HeaderPattern RequestLinePattern[] = {
    {"*", EMPTY, SPACE, 0, OFFSETOF(struct RequestLine, method), ParseString, SipMethodLegal, String2String},
    {"*", SPACE, SPACE, 0, OFFSETOF(struct RequestLine, requestUri), ParseUri, NULL, Uri2String},
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

int RequestLineSetUri(struct RequestLine *r, URI *u)
{
    assert(r != NULL);
    assert(u != NULL);
    
    if (r->requestUri != NULL)
        DestroyUri(&r->requestUri);
    r->requestUri = u;

    return 0;
}

URI *RequestLineGetUri(struct RequestLine *r)
{
    assert (r != NULL);
    return r->requestUri;
}

BOOL RequestLineMethodMatched(struct RequestLine *r1, struct RequestLine *r2)
{
    assert(r1 != NULL);
    assert(r2 != NULL);

    if (StrcmpExt(r1->method, SIP_METHOD_NAME_INVITE) == 0
        && StrcmpExt(r2->method, SIP_METHOD_NAME_ACK) == 0) {
        return TRUE;
    }
    
    if (StrcmpExt(r1->method, r2->method) != 0) {
        return FALSE;
    }

    return TRUE;
}

BOOL RequestLineMatched(struct RequestLine *r1, struct RequestLine *r2)
{
    assert(r1 != NULL);
    assert(r2 != NULL);

    if (!UriMatched(RequestLineGetUri(r1), RequestLineGetUri(r2))) {
        return FALSE;
    }

    if (!RequestLineMethodMatched(r1, r2)) {
        return FALSE;
    }

    if (StrcmpExt(r1->sipVersion, r2->sipVersion) != 0){
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

struct RequestLine *CreateRequestLine(SIP_METHOD m, URI *u)
{
    assert (u != NULL);

    struct RequestLine *r = CreateEmptyRequestLine();
    RequestLineSetMethod(r, MethodMap2String(m));    
    RequestLineSetUri(r, u);
    RequestLineSetSipVersion(r, SIP_VERSION);

    return r;
}

struct RequestLine *RequestLineDup(struct RequestLine *origin)
{
    struct RequestLine *dest = CreateEmptyRequestLine();
    RequestLineSetMethod(dest, RequestLineGetMethodName(origin));
    RequestLineSetUri(dest, UriDup(RequestLineGetUri(origin)));
    RequestLineSetSipVersion(dest, SIP_VERSION);
    return dest;
}

struct RequestLine *CreateEmptyRequestLine()
{
    struct RequestLine *requestLine = calloc (1, sizeof(struct RequestLine));
    requestLine->requestUri = CreateEmptyUri();
    return requestLine;
}

void DestroyRequestLine(struct RequestLine *requestLine)
{
    if (requestLine != NULL) {
        DestroyUri(&requestLine->requestUri);
        free(requestLine);
    }
}
