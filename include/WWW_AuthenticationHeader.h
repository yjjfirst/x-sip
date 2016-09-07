
#define AUTH_HEADER_ALGORITHM "algorithm"
#define ALGORITHM_MD5 "MD5"
#define AUTH_HEADER_URI "uri"
#define AUTH_HEADER_USER_NAME "username"
#define AUTH_HEADER_REALM "realm"
#define AUTH_HEADER_RESPONSE "response"
#define AUTH_HEADER_NONCE "nonce"

#define MD5_HASH_LENGTH 32

struct Header;
struct AuthHeader;

enum AuthScheme {
    BASIC,
    DIGEST,
};

void AuthHeaderSetScheme(struct AuthHeader *authHeader, enum AuthScheme scheme);
void AuthHeaderSetParameter(struct AuthHeader *authHeader, const char *name, const char *value);
enum AuthScheme AuthHeaderGetScheme(struct AuthHeader *authHeader);
struct Parameters *AuthHeaderGetParameters(struct AuthHeader *authHeader);
char *AuthHeaderGetParameter(struct AuthHeader *authHeader, const char *name);
void CalculateResponse(char *username, char *passwd, char *uri, char *realm, char *nonce, char *response);


struct Header *ParseAuthHeader(char *string);
char *AuthHeader2String(char *result, struct Header *auth);
struct AuthHeader *CreateAuthHeader();
struct AuthHeader *CreateWWW_AuthenticationHeader();
struct AuthHeader *CreateAuthorizationHeader();

void DestroyAuthHeader(struct Header *authHeader);
