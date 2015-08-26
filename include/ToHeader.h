#include "Parser.h"

struct ToHeader;

struct ParsePattern *GetToHeaderPattern(char *header);
struct ToHeader *CreateToHeader();
void DestoryToHeader(struct ToHeader *to);

char *ToHeaderGetName(struct ToHeader *toHeader);
char *ToHeaderGetDisplayName(struct ToHeader *toHeader);
char *ToHeaderGetUri(struct ToHeader *toHeader);
char *ToHeaderGetParameters(struct ToHeader *toHeader);

