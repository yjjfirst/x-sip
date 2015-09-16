struct URI;

void DestoryUri(struct URI *uri);
struct URI *CreateUri();

struct HeaderPattern *GetURIHeaderPattern (char *header);
char *UriGetScheme(struct URI *uri);
char *UriGetUser(struct URI *uri);
char *UriGetHost(struct URI *uri);
char *UriGetPort(struct URI *uri);
char *UriGetParameters(struct URI *uri);
char *UriGetHeaders(struct URI *uri);

char *Uri2String(char *string, void *uri, struct HeaderPattern *p);
char *Uri2StringExt(char *string, void *uri, struct HeaderPattern *p);
