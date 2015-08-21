#include <string.h>
#include "URI.h"
#include "Parser.h"

struct ParsePattern URIParsePattern[] = {
    { "scheme",  EMPTY,      COLON, 0, OFFSETOF(struct URI, scheme), ParseAtomElement},
    { "user",    COLON,      AT,    0, OFFSETOF(struct URI, user), ParseAtomElement},
    { "host",    AT,         ANY, 0, OFFSETOF(struct URI, host), ParseAtomElement},
    { "port",    COLON,      ANY, 0, OFFSETOF(struct URI, port), ParseAtomElement},
    { "parameters",SEMICOLON, ANY, 0, OFFSETOF(struct URI, parameters), ParseAtomElement},
    { "headers", QUESTION,   ANY,0, OFFSETOF(struct URI, headers), ParseAtomElement},
    {NULL, 0, 0, 0, 0}

};

struct ParsePattern URINoUserParsePattern[] = {
    { "scheme",  EMPTY,      COLON, 0, OFFSETOF(struct URI, scheme), ParseAtomElement},
    { "host",    COLON,      ANY, 0, OFFSETOF(struct URI, host), ParseAtomElement},
    { "port",    COLON,      ANY, 0, OFFSETOF(struct URI, port), ParseAtomElement},
    { "parameters",SEMICOLON, ANY, 0, OFFSETOF(struct URI, parameters), ParseAtomElement},
    { "headers", QUESTION,   ANY,0, OFFSETOF(struct URI, headers), ParseAtomElement},
    {NULL, 0, 0, 0, 0}

};

struct ParsePattern *GetURIParsePattern (char *header)
{
    if (strchr(header, '@') == NULL) {
        return URINoUserParsePattern;
    }
    else { 
        return URIParsePattern;
    }
}

