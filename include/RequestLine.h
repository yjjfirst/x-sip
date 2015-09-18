#include "URI.h"

struct RequestLine;

struct HeaderPattern *GetRequestLinePattern();   
void DestoryRequestLine(struct RequestLine *requestLine);
struct RequestLine *CreateRequestLine();

char *RequestLineGetMethod (struct RequestLine *r);
char *RequestLineGetSipVersion(struct RequestLine *r);
struct URI *RequestLineGetUri(struct RequestLine *r);

int RequestLineSetMethod(struct RequestLine *r, char *method);
int RequestLineSetSipVersion(struct RequestLine *r, char *version);
int RequestLineSetUri(struct RequestLine *r, struct URI *u);

char *RequestLine2String(char *string, struct RequestLine *r);
