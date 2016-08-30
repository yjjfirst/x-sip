struct Header;
struct AuthHeader;

enum AuthMechanism {
    BASIC,
    DIGEST,
};

enum AuthMechanism AuthHeaderGetMechanism(struct AuthHeader *authHeader);
struct Parameters *AuthHeaderGetParameters(struct AuthHeader *authHeader);

struct Header *ParseAuthHeader(char *string);
char *AuthHeader2String(char *result, struct Header *auth);
struct AuthHeader *CreateAuthHeader();
void DestroyAuthHeader(struct Header *authHeader);
