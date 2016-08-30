struct Header;
struct AuthHeader;

enum AuthMechanism {
    BASIC,
    DIGEST,
};

enum AuthMechanism AuthHeaderGetMechanism(struct AuthHeader *authHeader);
struct Parameters *AuthHeaderGetParameters(struct AuthHeader *authHeader);

struct Header *ParseAuthHeader(char *string);
struct AuthHeader *CreateAuthHeader();
void DestroyAuthHeader(struct Header *authHeader);
