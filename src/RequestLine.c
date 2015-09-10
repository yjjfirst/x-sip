#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "RequestLine.h"
#include "Parser.h"

#define METHOD_MAX_LENGTH 16

struct RequestLine {
    char method[METHOD_MAX_LENGTH];
    struct URI *requestUri;
    char sipVersion[16];
};

static int ParseURI(char *header, void *target)
{
    struct ParsePattern *pattern = GetURIParsePattern(header);
    struct URI **uri = target;

    Parse(header, *uri, pattern);
    
    return 0;
}

int RequestLineMethodValidate(char *method)
{
    if (strcmp(method, "REGISTER") == 0 || strcmp(method, "INVITE") == 0)
        return 0;
    return -1;
}

struct ParsePattern RequestLinePattern[] = {
    {"*", EMPTY, SPACE, 0, OFFSETOF(struct RequestLine, method), ParseStringElement, RequestLineMethodValidate},
    {"*", SPACE, SPACE, 0, OFFSETOF(struct RequestLine, requestUri), ParseURI},
    {"*", SPACE, EMPTY, 0, OFFSETOF(struct RequestLine, sipVersion), ParseStringElement},
    {NULL, 0, 0, 0, 0, 0}
};

struct ParsePattern *GetRequestLinePattern ()
{
    return RequestLinePattern;
}

char *RequestLineGetMethod (struct RequestLine *r)
{
    return r->method;
}

char *RequestLineGetSipVersion(struct RequestLine *r)
{
    return r->sipVersion;
}

struct URI *RequestLineGetUri(struct RequestLine *r)
{
    return r->requestUri;
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
