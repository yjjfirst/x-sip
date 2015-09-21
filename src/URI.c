#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "URI.h"
#include "Parser.h"

struct URI {
    char scheme[8];
    char user[32];
    char host[32];
    char port[8];
    char parameters[128];
    char headers[128];
};

struct HeaderPattern URIHeaderPattern[] = {
    { "*",  EMPTY,      COLON, 0, OFFSETOF(struct URI, scheme), ParseStringElement,NULL,StringElement2String},
    { "*",  COLON,      AT,    0, OFFSETOF(struct URI, user), ParseStringElement, NULL,StringElement2String},
    { "*",  AT,         ANY, 0, OFFSETOF(struct URI, host), ParseStringElement, NULL,StringElement2String},
    { "*",  COLON,      ANY, 0, OFFSETOF(struct URI, port), ParseStringElement, NULL,StringElement2String},
    { "*",  SEMICOLON, ANY, 0, OFFSETOF(struct URI, parameters), ParseStringElement, NULL,StringElement2String},
    { "*",  QUESTION,   ANY,0, OFFSETOF(struct URI, headers), ParseStringElement, NULL,StringElement2String},
    {NULL, 0, 0, 0, 0}

};

struct HeaderPattern URINoUserHeaderPattern[] = {
    { "*",  EMPTY,      COLON, 0, OFFSETOF(struct URI, scheme), ParseStringElement,NULL,StringElement2String},
    { "*",  COLON,      ANY, 0, OFFSETOF(struct URI, host), ParseStringElement,NULL,StringElement2String},
    { "*",  COLON,      ANY, 0, OFFSETOF(struct URI, port), ParseStringElement,NULL,StringElement2String},
    { "*",  SEMICOLON, ANY, 0, OFFSETOF(struct URI, parameters), ParseStringElement,NULL,StringElement2String},
    { "*",  QUESTION,   ANY,0, OFFSETOF(struct URI, headers), ParseStringElement,NULL,StringElement2String},
    {NULL, 0, 0, 0, 0}

};

char *UriGetScheme(struct URI *uri)
{
    return uri->scheme;
}
char *UriGetUser(struct URI *uri)
{
    return uri->user;
}
char *UriGetHost(struct URI *uri)
{
    return uri->host;
}
char *UriGetPort(struct URI *uri)
{
    return uri->port;
}
char *UriGetParameters(struct URI *uri)
{
    return uri->parameters;
}
char *UriGetHeaders(struct URI *uri)
{
    return uri->headers;
}

struct URI *CreateUri()
{
    struct URI *uri = (struct URI *)calloc(1, sizeof (struct URI));
    return uri;
}

void DestoryUri(struct URI *uri)
{
    free (uri);
}

struct HeaderPattern *GetURIHeaderPattern (char *header)
{
    if (strchr(header, '@') == NULL)
        return URINoUserHeaderPattern;
    else 
        return URIHeaderPattern;
}

struct HeaderPattern *GetURIHeaderPattern42String(struct URI **uri)
{
    if (strcmp ("", UriGetUser(*uri)) == 0) {
        return URINoUserHeaderPattern;
    }
    else {
        return URIHeaderPattern;
    }
}

char *Uri2String(char *string, void *uri, struct HeaderPattern *p)
{
    struct HeaderPattern *pattern = GetURIHeaderPattern42String(uri);
    struct URI **u = uri;
    
    if (p->startSeparator != EMPTY) {
        *string = p->startSeparator;
        string ++;
    }
    
    return ToString(string, *u, pattern);
}

char *Uri2StringExt(char *string, void *uri, struct HeaderPattern *p)
{
    struct HeaderPattern *pattern = GetURIHeaderPattern42String((struct URI **)&uri);
    return ToString(string, uri, pattern);
}


