#include "CppUTest/TestHarness.h"

extern "C" {
#include "Parameter.h"
#include "Header.h"
#include "WWW_AuthenticationHeader.h"
}

char WWW_AuthenticateString[] =  "WWW-Authenticate:Digest algorithm=MD5,realm=\"asterisk\",nonce=\"0d70776f\"";

TEST_GROUP(WWW_AuthenticationTestGroup)
{

};

TEST(WWW_AuthenticationTestGroup, CreateAuthHeaderTest)
{
    struct AuthHeader *authHeader = CreateAuthHeader();

    CHECK_FALSE(authHeader == NULL);

    DestroyAuthHeader((struct Header *)authHeader);
}

TEST(WWW_AuthenticationTestGroup, CreateWWW_AuthenticationHeaderTest)
{
    struct AuthHeader *h = CreateWWW_AuthenticationHeader();

    STRCMP_EQUAL(HEADER_NAME_WWW_AUTHENTICATE, HeaderGetName( (struct Header *)h));
    DestroyAuthHeader((struct Header *)h);
}

TEST(WWW_AuthenticationTestGroup, CreateAuthorizationHeaderTest)
{
    struct AuthHeader *h = CreateAuthorizationHeader();

    STRCMP_EQUAL(HEADER_NAME_AUTHORIZATION, HeaderGetName( (struct Header *)h));
    DestroyAuthHeader((struct Header *)h);
}

TEST(WWW_AuthenticationTestGroup, ParseAuthHeaderHeaderNameTest)
{
    struct AuthHeader *authHeader = (struct AuthHeader *)ParseAuthHeader(WWW_AuthenticateString);

    STRCMP_EQUAL("WWW-Authenticate", HeaderGetName((struct Header *)authHeader));

    DestroyAuthHeader((struct Header *)authHeader);
}

TEST(WWW_AuthenticationTestGroup, ParseAuthHeaderMechanismTest)
{
    struct AuthHeader *authHeader = (struct AuthHeader *)ParseAuthHeader(WWW_AuthenticateString);
    enum AuthScheme authMechanism = AuthHeaderGetScheme(authHeader);

    CHECK_EQUAL(DIGEST, authMechanism);

    DestroyAuthHeader((struct Header *)authHeader);
}

TEST(WWW_AuthenticationTestGroup, ParseAuthHeaderParametersTest)
{
    struct AuthHeader *authHeader = (struct AuthHeader *)ParseAuthHeader(WWW_AuthenticateString);
    struct Parameters *ps  = AuthHeaderGetParameters(authHeader);

    STRCMP_EQUAL("MD5", GetParameter(ps, (char *)"algorithm"));
    STRCMP_EQUAL("\"asterisk\"", GetParameter(ps, (char *)"realm"));
    STRCMP_EQUAL("\"0d70776f\"", GetParameter(ps, (char *)"nonce"));

    DestroyAuthHeader((struct Header *)authHeader);
}

TEST(WWW_AuthenticationTestGroup, AuthHeader2StringTest)
{
    struct AuthHeader *authHeader = (struct AuthHeader *)ParseAuthHeader(WWW_AuthenticateString);
    char result[128] = {0};
    
    AuthHeader2String(result, (struct Header *)authHeader);

    STRCMP_EQUAL(WWW_AuthenticateString, result);
    DestroyAuthHeader((struct Header *)authHeader);
}

