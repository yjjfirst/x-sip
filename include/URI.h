struct URI;

void DestoryUri(struct URI *uri);
struct URI *CreateUri();

struct ParsePattern *GetURIParsePattern (char *header);
char *UriGetScheme(struct URI *uri);
char *UriGetUser(struct URI *uri);
char *UriGetHost(struct URI *uri);
char *UriGetPort(struct URI *uri);
char *UriGetParameters(struct URI *uri);
char *UriGetHeaders(struct URI *uri);
