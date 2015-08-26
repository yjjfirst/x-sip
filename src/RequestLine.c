#include <string.h>
#include <stdio.h>

#include <stdlib.h>
#include "RequestLine.h"

struct RequestLine {
    char method[16];
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

struct ParsePattern RequestLinePattern[] = {
    {"*", EMPTY, SPACE, 0, OFFSETOF(struct RequestLine, method), ParseAtomElement},
    {"*", SPACE, SPACE, 0, OFFSETOF(struct RequestLine, requestUri), ParseURI},
    {"*", SPACE, EMPTY, 0, OFFSETOF(struct RequestLine, sipVersion), ParseAtomElement},
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
        free(requestLine->requestUri);
        free(requestLine);
    }
}
