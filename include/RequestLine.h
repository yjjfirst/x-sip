#include "Parser.h"
#include "URI.h"

struct RequestLine {
    char Method[20];
    struct URI Request_URI;
    char SIP_Version[16];
};

struct ParsePattern *GetRequestLinePattern();   
