#include <string.h>
#include <stdio.h>

#include "WWW_AuthenticationHeader.h"
#include "Parser.h"
#include "Header.h"
#include "Parameter.h"
#include "StringExt.h"

struct AuthHeader {
    struct Header headerBase;
    char mechanism[32];
    struct Parameters *parameters;
};

struct HeaderPattern AuthHeaderPattern[] = {
    {"*", EMPTY, COLON, 0, OFFSETOF(struct AuthHeader, headerBase),ParseString, NULL, String2String},
    {"*", COLON, SPACE, 0, OFFSETOF(struct AuthHeader, mechanism), ParseString, NULL, String2String},
    {"*", SPACE, EMPTY, 0, OFFSETOF(struct AuthHeader, parameters), ParseParameters, NULL, Parameters2String},
    {NULL},
};

enum AuthMechanism AuthHeaderGetMechanism(struct AuthHeader *authHeader)
{
    assert(authHeader != NULL);

    if (StrcmpExt("Digest", authHeader->mechanism) == 0) {
        return DIGEST;
    } else {
        return BASIC;
    }
}

struct Parameters *AuthHeaderGetParameters(struct AuthHeader *authHeader)
{
    assert(authHeader != NULL);
    return authHeader->parameters;
}

struct Header *ParseAuthHeader(char *string)
{
    struct AuthHeader *authHeader = CreateAuthHeader();
    Parse(string, authHeader, AuthHeaderPattern);

    return (struct Header *) authHeader;
}

struct AuthHeader *CreateAuthHeader()
{
    struct AuthHeader *authHeader;

    authHeader = calloc(1, sizeof(struct AuthHeader));
    authHeader->parameters = CreateParameters();

    return authHeader;
}

void DestroyAuthHeader(struct Header *header)
{
    struct AuthHeader *authHeader =(struct AuthHeader *) header;

    if (authHeader == NULL) return;

    DestroyParameters(authHeader->parameters);
    free(authHeader);
}
