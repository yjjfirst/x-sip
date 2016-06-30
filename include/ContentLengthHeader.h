struct ContentLengthHeader;

struct ContentLengthHeader *CreateContentLengthHeader();
void DestroyContentLengthHeader(struct Header *header);
struct HeaderPattern *GetContentLengthHeaderPattern();
struct Header *ParseContentLengthHeader();

int ContentLengthHeaderGetLength(struct ContentLengthHeader *c);
void ContentLengthHeaderSetLength(struct ContentLengthHeader *c, int length);
char *ContentLengthHeaderGetName(struct ContentLengthHeader *c);

char *ContentLengthHeader2String(char *result, struct Header *c);

