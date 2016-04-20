struct Headers *CreateHeaders();
void DestoryHeaders(struct Headers **headers);
int HeadersLength(struct Headers *headers);

void HeadersAddHeader(struct Headers *headers, struct Header *header);


