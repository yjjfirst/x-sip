struct URI;

void DestoryUri(struct URI *uri);
struct URI *CreateUri();

struct HeaderPattern *GetURIHeaderPattern (char *header);

char *UriGetScheme(struct URI *uri);
char *UriGetUser(struct URI *uri);
char *UriGetHost(struct URI *uri);
int UriGetPort(struct URI *uri);
char *UriGetParameters(struct URI *uri);
char *UriGetHeaders(struct URI *uri);

void UriSetScheme(struct URI *uri, char *scheme);
void UriSetUser(struct URI *uri, char *user);
void UriSetHost(struct URI *uri, char *host);
void UriSetHeaders(struct URI *uri, char *headers);
void UriSetParameters(struct URI *uri,char *paramater);
void UriSetPort(struct URI *uri, int port);


char *Uri2String(char *string, void *uri, struct HeaderPattern *p);
char *Uri2StringExt(char *string, void *uri, struct HeaderPattern *p);










