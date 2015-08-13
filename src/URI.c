#include <string.h>
#include "URI.h"
#include "Parser.h"

int ParseScheme(char *value, void *target)
{
    struct URI *uri = (struct URI *) target;
    strcpy(uri->scheme, value);
}

struct ParsePattern URIParsePattern[] = {
    { "scheme", COLON, 0, ParseScheme},
    {NULL, NULL, 0, NULL}

};

struct ParsePattern *GetURIParsePattern ()
{
    return URIParsePattern;
}
