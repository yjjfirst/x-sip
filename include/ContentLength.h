struct ContentLengthHeader;

struct ContentLengthHeader *CreateContentLengthHeader();
void DestoryContentLengthHeader(struct Header *header);
struct HeaderPattern *GetContentLengthHeaderPattern();
struct Header *ParseContentLength();

int ContentLengthGetLength(struct ContentLengthHeader *c);
char *ContentLengthGetName(struct ContentLengthHeader *c);

void ContentLengthHeader2String(char *result, struct ContentLengthHeader *c);
