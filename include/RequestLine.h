#include "Parser.h"
#include "URI.h"

struct RequestLine;

struct ParsePattern *GetRequestLinePattern();   
void DestoryRequestLine(struct RequestLine *requestLine);
struct RequestLine *CreateRequestLine();

char *RequestLineGetMethod (struct RequestLine *r);
char *RequestLineGetSipVersion(struct RequestLine *r);
struct URI *RequestLineGetUri(struct RequestLine *r);

