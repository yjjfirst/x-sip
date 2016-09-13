#include "Bool.h"

#define URI_SCHEME_SIP "sip"
#define URI_SCHEME_SIPS "sips"

#define URI_MAX_ELEMENT 6
#define URI_SCHEME_MAX_LENGTH 8
#define URI_USER_MAX_LENGTH 32
#define URI_HOST_MAX_LENGTH 128
#define URI_STRING_MAX_LENGTH 256 

struct URI;
struct HeaderPattern;
struct Parameters;

typedef struct URI URI;

void DestroyUri(struct URI **uri);
struct URI *CreateEmptyUri();
struct URI *CreateUri(char *scheme, char *user, char *host, int port);
struct URI *UriDup(struct URI *src);
int ParseUri(char *header, void *target);

char *UriGetScheme(struct URI *uri);
char *UriGetUser(struct URI *uri);
char *UriGetHost(struct URI *uri);
int UriGetPort(struct URI *uri);
char *UriGetParameter(struct URI *uri, char *name);
char *UriGetHeader(struct URI *uri, char *name);
BOOL UriMatched(struct URI *uri, struct URI *uri2);

void UriSetScheme(struct URI *uri, char *scheme);
void UriSetUser(struct URI *uri, char *user);
void UriSetHost(struct URI *uri, char *host);
void UriSetHeaders(struct URI *uri, struct Parameters *headers);
void UriSetParameters(struct URI *uri,struct Parameters  *paramaters);
void UriAddParameter(struct URI *uri, char *name, char *value);
void UriSetPort(struct URI *uri, int port);

char *Uri2String(char *string, void *uri, struct HeaderPattern *p);
char *Uri2StringExt(char *string, void *uri);
