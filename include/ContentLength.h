#include "Parser.h"
struct ContentLengthHeader;

struct ContentLengthHeader *CreateContentLengthHeader();
void DestoryContentLengthHeader(struct ContentLengthHeader *header);
struct ParsePattern *GetContentLengthParsePattern();
int ContentLengthGetLength(struct ContentLengthHeader *c);
char *ContentLengthGetName(struct ContentLengthHeader *c);
