#include <string.h>
#include <stdio.h>

#include "WWW_AuthenticationHeader.h"
#include "Parser.h"
#include "Header.h"
#include "Parameter.h"
#include "StringExt.h"

#define AUTHENTICATION_SCHEME_DIGEST "Digest"
#define AUTHENTICATION_SCHEME_BASIC "Basic"

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

void AuthHeaderSetScheme(struct AuthHeader *authHeader, enum AuthScheme scheme)
{
    assert(authHeader != NULL);
    
    struct HeaderPattern *p = &AuthHeaderPattern[1];
    if (scheme == DIGEST)
        Copy2Target(authHeader, AUTHENTICATION_SCHEME_DIGEST, p);
    else
        Copy2Target(authHeader, AUTHENTICATION_SCHEME_BASIC, p);
}

enum AuthScheme AuthHeaderGetScheme(struct AuthHeader *authHeader)
{
    assert(authHeader != NULL);

    if (StrcmpExt(AUTHENTICATION_SCHEME_DIGEST, authHeader->mechanism) == 0) {
        return DIGEST;
    } else {
        return BASIC;
    }
}

BOOL QuotedValue(const char *value)
{
    return value[0] != '\"' && value[strlen(value) -1] != '\"';
}

void AuthHeaderSetParameter(struct AuthHeader *authHeader, const char *name, const char *value)
{
    struct Parameters *ps = AuthHeaderGetParameters(authHeader);
    char quotedValue[PARAMETER_VALUE_MAX_LENGTH] = {0};

    if (strcmp(name, AUTH_HEADER_ALGORITHM) != 0 && QuotedValue(value)) {
        quotedValue[0] = '\"';
        strcpy(quotedValue + 1, value);
        quotedValue[strlen(quotedValue)] = '\"';
        AddParameter(ps, (char *)name, quotedValue);
    } else {
        AddParameter(ps, (char *)name, (char *)value);
    }
    
}

char *AuthHeaderGetParameter(struct AuthHeader *authHeader, const char *name)
{
    struct Parameters *ps = AuthHeaderGetParameters(authHeader);

    return GetParameter(ps, (char *)name);    
}

struct Parameters *AuthHeaderGetParameters(struct AuthHeader *authHeader)
{
    assert(authHeader != NULL);
    return authHeader->parameters;
}

char *AuthHeader2String(char *result, struct Header *auth)
{
    assert(result != NULL);
    assert(auth != NULL);
    
    char *end = ToString(result, auth, AuthHeaderPattern);
    int len = strlen(result);

    for (int i = 0; i < len; i++) {
        if (result[i] == SEMICOLON)
            result[i] =',';
    }
    
    return end;
}

struct Header *ParseAuthHeader(char *string)
{
    struct AuthHeader *authHeader = CreateAuthHeader();
    Parse(string, authHeader, AuthHeaderPattern);

    return (struct Header *) authHeader;
}

void AuthHeaderSetName(struct AuthHeader *header, char *name)
{
    struct HeaderPattern *p = AuthHeaderPattern;
    Copy2Target(header, name, p);
}

struct AuthHeader *CreateWWW_AuthenticationHeader()
{
    struct AuthHeader *authHeader = CreateAuthHeader();

    AuthHeaderSetName(authHeader, HEADER_NAME_WWW_AUTHENTICATE);
    return authHeader;
}

struct AuthHeader *CreateAuthorizationHeader()
{
    struct AuthHeader *authHeader = CreateAuthHeader();

    AuthHeaderSetName(authHeader, HEADER_NAME_AUTHORIZATION);
    return authHeader;
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
