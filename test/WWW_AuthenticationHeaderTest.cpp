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

TEST(WWW_AuthenticationTestGroup, SetSchemeTest)
{
    struct AuthHeader *h  = CreateAuthorizationHeader();

    AuthHeaderSetScheme(h, DIGEST);
    CHECK_EQUAL(DIGEST, AuthHeaderGetScheme(h));

    AuthHeaderSetScheme(h, BASIC);
    CHECK_EQUAL(BASIC, AuthHeaderGetScheme(h));
    

    DestroyAuthHeader((struct Header *) h);
}

TEST(WWW_AuthenticationTestGroup, ParametersAlgorithmTest)
{
    struct AuthHeader *h  = CreateAuthorizationHeader();

    AuthHeaderSetParameter(h, AUTH_HEADER_ALGORITHM, ALGORITHM_MD5);
    STRCMP_EQUAL(ALGORITHM_MD5, AuthHeaderGetParameter(h, AUTH_HEADER_ALGORITHM));
    
    DestroyAuthHeader((struct Header *) h);
}

TEST(WWW_AuthenticationTestGroup, ParametersUriTest)
{
    struct AuthHeader *h  = CreateAuthorizationHeader();

    AuthHeaderSetParameter(h, AUTH_HEADER_URI, "sip://192.168.10.62");
    STRCMP_EQUAL("\"sip://192.168.10.62\"", AuthHeaderGetParameter(h, AUTH_HEADER_URI));
    
    DestroyAuthHeader((struct Header *) h);
}

TEST(WWW_AuthenticationTestGroup, ParametersUserNameTest)
{
    struct AuthHeader *h  = CreateAuthorizationHeader();

    AuthHeaderSetParameter(h, AUTH_HEADER_USER_NAME, "88004");
    STRCMP_EQUAL("\"88004\"", AuthHeaderGetParameter(h, AUTH_HEADER_USER_NAME));
    
    DestroyAuthHeader((struct Header *) h);
}

TEST(WWW_AuthenticationTestGroup, ParametersRealmTest)
{
    struct AuthHeader *h  = CreateAuthorizationHeader();

    AuthHeaderSetParameter(h, AUTH_HEADER_REALM, "asterisk");
    STRCMP_EQUAL("\"asterisk\"", AuthHeaderGetParameter(h, AUTH_HEADER_REALM));
    
    DestroyAuthHeader((struct Header *) h);
}

TEST(WWW_AuthenticationTestGroup, ParametersResponseTest)
{
    struct AuthHeader *h  = CreateAuthorizationHeader();

    AuthHeaderSetParameter(h, AUTH_HEADER_RESPONSE, "ff880a705d5848ea0b81bdfbce0ea782");
    STRCMP_EQUAL("\"ff880a705d5848ea0b81bdfbce0ea782\"", AuthHeaderGetParameter(h, AUTH_HEADER_RESPONSE));
    
    DestroyAuthHeader((struct Header *) h);
}

TEST(WWW_AuthenticationTestGroup, ParametersNonceTest)
{
    struct AuthHeader *h  = CreateAuthorizationHeader();

    AuthHeaderSetParameter(h, AUTH_HEADER_NONCE, "ff880a705");
    STRCMP_EQUAL("\"ff880a705\"", AuthHeaderGetParameter(h, AUTH_HEADER_NONCE));
    
    DestroyAuthHeader((struct Header *) h);
}


TEST(WWW_AuthenticationTestGroup, SetParameterQuotedTest)
{
    struct AuthHeader *h  = CreateAuthorizationHeader();

    AuthHeaderSetParameter(h, AUTH_HEADER_NONCE, "\"ff880a705\"");
    STRCMP_EQUAL("\"ff880a705\"", AuthHeaderGetParameter(h, AUTH_HEADER_NONCE));
    
    DestroyAuthHeader((struct Header *) h);

}

TEST(WWW_AuthenticationTestGroup, AllParametersTest)
{
    struct AuthHeader *h  = CreateAuthorizationHeader();

    AuthHeaderSetParameter(h, AUTH_HEADER_ALGORITHM, ALGORITHM_MD5);
    STRCMP_EQUAL(ALGORITHM_MD5, AuthHeaderGetParameter(h, AUTH_HEADER_ALGORITHM));

    AuthHeaderSetParameter(h, AUTH_HEADER_URI, "sip://192.168.10.62");
    STRCMP_EQUAL("\"sip://192.168.10.62\"", AuthHeaderGetParameter(h, AUTH_HEADER_URI));

    AuthHeaderSetParameter(h, AUTH_HEADER_USER_NAME, "88004");
    STRCMP_EQUAL("\"88004\"", AuthHeaderGetParameter(h, AUTH_HEADER_USER_NAME));

    AuthHeaderSetParameter(h, AUTH_HEADER_REALM, "asterisk");
    STRCMP_EQUAL("\"asterisk\"", AuthHeaderGetParameter(h, AUTH_HEADER_REALM));

    AuthHeaderSetParameter(h, AUTH_HEADER_RESPONSE, "ff880a705d5848ea0b81bdfbce0ea782");
    STRCMP_EQUAL("\"ff880a705d5848ea0b81bdfbce0ea782\"", AuthHeaderGetParameter(h, AUTH_HEADER_RESPONSE));
    
    AuthHeaderSetParameter(h, AUTH_HEADER_NONCE, "ff880a705");
    STRCMP_EQUAL("\"ff880a705\"", AuthHeaderGetParameter(h, AUTH_HEADER_NONCE));

    DestroyAuthHeader((struct Header *) h);
}

TEST(WWW_AuthenticationTestGroup, CalculateResponaseTest)
{
    char username[] = "88004";
    char passwd[] = "88004";
    char uri[] = "sip:192.168.10.62";
    char realm[] = "asterisk";
    char nonce[] = "40062bed";
    char response[64] = {0};
    
    CalculateResponse(username, passwd, uri, realm, nonce, response);
    STRCMP_EQUAL("ff880a705d5848ea0b81bdfbce0ea782", response);
}
