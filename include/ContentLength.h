struct ContentLengthHeader;

struct ContentLengthHeader *CreateContentLengthHeader();
void DestoryContentLengthHeader(struct Header *header);
struct ParsePattern *GetContentLengthParsePattern();
struct Header *ParseContentLength();

int ContentLengthGetLength(struct ContentLengthHeader *c);
char *ContentLengthGetName(struct ContentLengthHeader *c);
