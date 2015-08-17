#include <string.h>
#include "URI.h"
#include "Parser.h"

struct ParsePattern URIParsePattern[] = {
    { "scheme",  EMPTY,      COLON, 0, OFFSETOF(struct URI, scheme)},
    { "user",    COLON,      AT,    1, OFFSETOF(struct URI, user)},
    { "host",    AT,         ANY, 0, OFFSETOF(struct URI, host)},
    { "port",    COLON,      ANY, 1, OFFSETOF(struct URI, port)},
    { "parameters",SEMICOLON, ANY, 1, OFFSETOF(struct URI, parameters)},
    { "headers", QUESTION,   ANY,1, OFFSETOF(struct URI, headers)},
    {NULL, 0, 0, 0, 0}

};

struct ParsePattern *GetURIParsePattern ()
{
    return URIParsePattern;
}

