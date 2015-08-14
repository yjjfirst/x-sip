#include <string.h>
#include "URI.h"
#include "Parser.h"

struct ParsePattern URIParsePattern[] = {
    { "scheme", COLON, 0, OFFSETOF(struct URI, scheme)},
    { "user",   AT,    0, OFFSETOF(struct URI, user)},
    { "host",   COLON, 1, OFFSETOF(struct URI, host)},
    { "port",   SEMICOLON, 1, OFFSETOF(struct URI, port)},
    { "parameters", QUESTION, 1, OFFSETOF(struct URI, parameters)},
    { "headers", EMPTY,1, OFFSETOF(struct URI, headers)},
    {NULL, NULL, 0, 0}

};

struct ParsePattern *GetURIParsePattern ()
{
    return URIParsePattern;
}

