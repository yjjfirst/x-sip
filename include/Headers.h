#include "utils/list/include/list.h"

struct Headers *CreateHeaders();
void DestoryHeaders(struct Headers **headers);
int HeadersLength(struct Headers *headers);

void HeadersAddHeader(struct Headers *headers, struct Header *header);
struct Header *HeadersGetHeader(const char *name, struct Headers *headers);

void RawHeadersAddHeader(struct Headers *headers, struct Header *header);
struct Header *RawHeadersGetHeader(const char *name, struct Headers *headers);
void RawParseHeader(char *string, struct Headers *headers);
char *RawHeaders2String(char *result, struct Headers *headers);
void RawDestoryHeaders(struct Headers *headers);
