#include <string.h>
#include "URI.h"
#include "Parser.h"

struct ParsePattern URIParsePattern[] = {
    { "scheme", COLON, 0, OFFSETOF(struct URI, scheme)},
    {NULL, NULL, 0, NULL}

};

struct ParsePattern *GetURIParsePattern ()
{
    return URIParsePattern;
}

