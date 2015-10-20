#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "URI.h"
#include "Parser.h"

#define URI_MAX_ELEMENT 6

struct URI {
    char scheme[8];
    char user[32];
    char host[32];
    int  port;
    char parameters[128];
    char headers[128];
};

struct HeaderPattern UserPattern[] = {
    { "*",  COLON,      AT,    0, OFFSETOF(struct URI, user), ParseString, NULL,String2String},
};

struct HeaderPattern HostPattern[] = {
    { "*",  COLON,      ANY, 0, OFFSETOF(struct URI, host), ParseString,NULL,String2String},
};

struct HeaderPattern URISchemePattern[] = {
    { "*",  EMPTY,      COLON, 0, OFFSETOF(struct URI, scheme), ParseString,NULL,String2String},
};

struct HeaderPattern URIRemainPattern[] = {
    { "*",  COLON,      ANY, 0, OFFSETOF(struct URI, port), ParseInteger,NULL,Integer2String},
    { "*",  SEMICOLON, ANY, 0, OFFSETOF(struct URI, parameters), ParseString,NULL,String2String},
    { "*",  QUESTION,   ANY,0, OFFSETOF(struct URI, headers), ParseString,NULL,String2String},
};

struct HeaderPattern URIPattern[URI_MAX_ELEMENT + 1];

int HasUser4Parse(void *h)
{
    char *header = (char *)h;
    
    if (strchr(header, '@') != NULL)
        return TRUE;

    return FALSE;
}

int HasUser42String(void *u)
{
    struct URI **uri = (struct URI **)(u);

    if (strcmp("", UriGetUser(*uri)) == 0)
        return FALSE;

    return TRUE;
}

#define COUNT_ELEMENT(pattern) (sizeof(pattern)/sizeof(struct HeaderPattern)) 

struct HeaderPattern *AddSchemePattern(struct HeaderPattern *pos)
{
    memcpy(pos, URISchemePattern, sizeof(URISchemePattern));
    pos += COUNT_ELEMENT(URISchemePattern);

    return pos;
}

struct HeaderPattern *AddUserHostPattern(struct HeaderPattern *pos, int hasUser)
{
    if (hasUser == TRUE){
        memcpy(pos, UserPattern, sizeof(UserPattern));
        pos += COUNT_ELEMENT(UserPattern);

        HostPattern[0].startSeparator = AT;
        memcpy(pos, HostPattern, sizeof(HostPattern));
        pos += sizeof(HostPattern)/sizeof(struct HeaderPattern);
    } else {
        HostPattern[0].startSeparator = COLON;
        memcpy(pos, HostPattern, sizeof(HostPattern));
        pos += COUNT_ELEMENT(HostPattern);
    }

    return pos;
}

struct HeaderPattern *AddRemainPattern(struct HeaderPattern *pos)
{
    memcpy(pos, URIRemainPattern, sizeof(URIRemainPattern));
    pos += COUNT_ELEMENT(URIRemainPattern);

    return pos;
}

struct HeaderPattern *GetURIPattern (void *uri, int (*hasUser)(void *cond))
{
    struct HeaderPattern *next = URIPattern;
    int has = hasUser(uri);
    
    next = AddSchemePattern(next);
    next = AddUserHostPattern(next, has);
    next = AddRemainPattern(next);    
    next->format = NULL;

    return URIPattern;
}

struct HeaderPattern *GetURIPattern4Parse(char *string)
{
    return GetURIPattern((void *)string, HasUser4Parse);
}

struct HeaderPattern *GetURIPattern42String(struct URI **uri)
{
    return GetURIPattern((void *)uri, HasUser42String);
}

int ParseURI(char *string, void *target)
{
    struct HeaderPattern *pattern = GetURIPattern4Parse(string);
    struct URI **uri = target;

    Parse(string, *uri, pattern);
    
    return 0;
}

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

int UriGetPort(struct URI *uri)
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

void UriSetScheme(struct URI *uri, char *scheme)
{
    struct HeaderPattern *p = &URISchemePattern[0];
    Copy2Target(uri, scheme, p);
}

void UriSetUser(struct URI *uri, char *user)
{
    struct HeaderPattern *p = UserPattern;
    Copy2Target(uri, user, p);

}

void UriSetHost(struct URI *uri, char *host)
{
    struct HeaderPattern *p = HostPattern;
    Copy2Target(uri, host, p);
}

void UriSetPort(struct URI *uri, int port)
{
    struct HeaderPattern *p = &URIRemainPattern[0];

    SetIntegerField(uri, port, p);
}

void UriSetParameters(struct URI *uri,char *paramater)
{
    struct HeaderPattern *p = &URIRemainPattern[1];
    Copy2Target(uri, paramater, p);
}

void UriSetHeaders(struct URI *uri, char *headers)
{
    struct HeaderPattern *p = &URIRemainPattern[2];
    Copy2Target(uri, headers, p);
}

struct URI *CreateEmptyUri()
{
    struct URI *uri = calloc(1, sizeof (struct URI));
    return uri;
}

struct URI *CreateUri(char *scheme, char *user, char *host, int port)
{
    struct URI *uri = CreateEmptyUri();
    UriSetScheme(uri, scheme);
    UriSetUser(uri, user);
    UriSetHost(uri, host);
    UriSetPort(uri, port);

    return uri;
}

struct URI *UriDup(struct URI *uri)
{
    struct URI *dupUri = CreateEmptyUri();
    
    memcpy(dupUri, uri, sizeof (struct URI));
    return dupUri;
}
void DestoryUri(struct URI *uri)
{
    free (uri);
}

char *Uri2String(char *string, void *uri, struct HeaderPattern *p)
{
    struct HeaderPattern *pattern = GetURIPattern42String(uri);
    struct URI **u = uri;
    
    if (p->startSeparator != EMPTY) {
        *string = p->startSeparator;
        string ++;
    }
    
    return ToString(string, *u, pattern);
}

char *Uri2StringExt(char *string, void *uri)
{
    struct HeaderPattern *pattern = GetURIPattern42String((struct URI **)&uri);
    return ToString(string, uri, pattern);
}
