#include "utils/list/include/list.h"

struct Headers *CreateHeaders();
void DestoryHeaders(struct Headers **headers);
int HeadersLength(struct Headers *headers);

void HeadersAddHeader(struct Headers *headers, struct Header *header);
struct Header *HeadersGetHeader(const char *name, struct Headers *headers);
//void ParseHeader(char *headerString, struct Headers *headers);
//char *Headers2String(char *p, struct Headers *headers);

void RawHeadersAddHeader(t_list *headers, struct Header *header);
struct Header *RawHeadersGetHeader(const char *name, t_list *headers);
void RawParseHeader(char *string, t_list *headers);
char *RawHeaders2String(char *result, t_list *headers);
void RawDestoryHeaders(t_list *headers);
