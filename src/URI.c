#include <string.h>
#include "URI.h"

  
int ParseURI(char *URIString, struct URI *uri)
{
    strcpy(uri->scheme, "sip");
    strcpy(uri->user,"alice");
    return 0;
}
