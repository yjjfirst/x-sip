#include <string.h>
#include <stdio.h>

#include "WWW_AuthenticationHeader.h"
#include "Parser.h"
#include "Header.h"
#include "Parameter.h"
#include "StringExt.h"
#include "utils/md5.h"

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

BOOL UnquotedValue(const char *value)
{
    return value[0] != '\"' && value[strlen(value) -1] != '\"';
}

void AuthHeaderSetParameter(struct AuthHeader *authHeader, const char *name, const char *value)
{
    struct Parameters *ps = AuthHeaderGetParameters(authHeader);
    char quotedValue[PARAMETER_VALUE_MAX_LENGTH] = {0};

    if (strcmp(name, AUTH_HEADER_ALGORITHM) != 0 && UnquotedValue(value)) {
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

#define HASHLEN 16
#define HASHHEXLEN 32
typedef unsigned char HASH[HASHLEN];
typedef char HASHHEX[HASHHEXLEN+1];

void convert_to_hex(HASH Bin, HASHHEX Hex )
{
    unsigned short i;
    unsigned char j;

    for (i = 0; i < HASHLEN; i++) {
        j = (Bin[i] >> 4) & 0xf;
        if (j <= 9)
            Hex[i*2] = (j + '0');
        else
            Hex[i*2] = (j + 'a' - 10);
        j = Bin[i] & 0xf;
        if (j <= 9)
            Hex[i*2+1] = (j + '0');
        else
            Hex[i*2+1] = (j + 'a' - 10);
    };
    Hex[HASHHEXLEN] = '\0';
};

void UnquoteString(char *dest, char *src)
{
    strcpy(dest, src + 1);
    dest[strlen(dest) -1] = 0;
}

void CalculateResponse(char *username, char *passwd, char *uri, char *realm, char *nonce, char *response)
{
    char a1[128] = {0};
    char a2[128] = {0};
    char final[128] = {0};
    
    MD5_CTX Md5Ctx;
    HASH HA1;
    HASHHEX HA1Hex;
    HASH HA2;
    HASHHEX HA2Hex;
    HASH HFinal;
    HASHHEX HFinalHex;

    char unquotedRealm[PARAMETER_VALUE_MAX_LENGTH];
    char unquotedNonce[PARAMETER_VALUE_MAX_LENGTH];
    char unquotedUri[PARAMETER_VALUE_MAX_LENGTH];
    
    UnquoteString(unquotedRealm, realm);
    UnquoteString(unquotedUri, uri);
    UnquoteString(unquotedNonce, nonce);

    snprintf(a1, sizeof(a1) - 1, "%s:%s:%s", username, unquotedRealm, passwd);    
    snprintf(a2, sizeof(a2) - 1, "%s:%s", "REGISTER", unquotedUri);    
    
    MD5_Init(&Md5Ctx);
    MD5_Update(&Md5Ctx, a1, strlen(a1));
    MD5_Final(HA1, &Md5Ctx);
    convert_to_hex(HA1, HA1Hex);
    //printf("%s->%s\n",a1, HA1Hex);
    
    MD5_Init(&Md5Ctx);
    MD5_Update(&Md5Ctx, a2, strlen(a2));
    MD5_Final(HA2, &Md5Ctx);
    convert_to_hex(HA2, HA2Hex);
    //printf("%s->%s\n",a2, HA2Hex);
 
    
    snprintf(final, sizeof(final) -1, "%s:%s:%s", HA1Hex, unquotedNonce, HA2Hex);
    MD5_Init(&Md5Ctx);
    MD5_Update(&Md5Ctx, final , strlen(final));
    MD5_Final(HFinal, &Md5Ctx);
    convert_to_hex(HFinal, HFinalHex);
    //printf("%s->%s\n", final, HFinalHex);
    strcpy(response, HFinalHex);
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

    DestroyParameters(&authHeader->parameters);
    free(authHeader);
}
