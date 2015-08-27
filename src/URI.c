#include <string.h>
#include <stdlib.h>

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

struct ParsePattern URIParsePattern[] = {
    { "*",  EMPTY,      COLON, 0, OFFSETOF(struct URI, scheme), ParseStringElement},
    { "*",  COLON,      AT,    0, OFFSETOF(struct URI, user), ParseStringElement},
    { "*",  AT,         ANY, 0, OFFSETOF(struct URI, host), ParseStringElement},
    { "*",  COLON,      ANY, 0, OFFSETOF(struct URI, port), ParseStringElement},
    { "*",  SEMICOLON, ANY, 0, OFFSETOF(struct URI, parameters), ParseStringElement},
    { "*",  QUESTION,   ANY,0, OFFSETOF(struct URI, headers), ParseStringElement},
    {NULL, 0, 0, 0, 0}

};

struct ParsePattern URINoUserParsePattern[] = {
    { "*",  EMPTY,      COLON, 0, OFFSETOF(struct URI, scheme), ParseStringElement},
    { "*",  COLON,      ANY, 0, OFFSETOF(struct URI, host), ParseStringElement},
    { "*",  COLON,      ANY, 0, OFFSETOF(struct URI, port), ParseStringElement},
    { "*",  SEMICOLON, ANY, 0, OFFSETOF(struct URI, parameters), ParseStringElement},
    { "*",  QUESTION,   ANY,0, OFFSETOF(struct URI, headers), ParseStringElement},
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

struct ParsePattern *GetURIParsePattern (char *header)
{
    if (strchr(header, '@') == NULL) {
        return URINoUserParsePattern;
    }
    else { 
        return URIParsePattern;
    }
}

