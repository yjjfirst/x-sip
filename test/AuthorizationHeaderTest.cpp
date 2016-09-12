#include "CppUTest/TestHarness.h"

extern "C" {
#include <stdio.h>
#include "Parameter.h"
#include "Header.h"
#include "WWW_AuthenticationHeader.h"
}

char AuthorizationString[] = "Authorization:Digest username=\"88004\",realm=\"asterisk\",nonce=\"40062bed\",uri=\"sip:192.168.10.62\",response=\"ff880a705d5848ea0b81bdfbce0ea782\",algorithm=MD5";


TEST_GROUP(AuthorizationTestGroup)
{

};

TEST(AuthorizationTestGroup, ParseAuthorizationHeaderHeaderNameTest)
{
    struct AuthHeader *authHeader = (struct AuthHeader *)ParseAuthHeader(AuthorizationString);

    STRCMP_EQUAL("Authorization", HeaderGetName((struct Header *)authHeader));
    
    DestroyAuthHeader((struct Header *)authHeader);
}

TEST(AuthorizationTestGroup, ParseAuthorizationHeaderParametersTest)
{
    struct AuthHeader *authHeader = (struct AuthHeader *)ParseAuthHeader(AuthorizationString);
    struct Parameters *ps  = AuthHeaderGetParameters(authHeader);
    
    STRCMP_EQUAL("\"88004\"", GetParameter(ps, (char*)"username"));
    STRCMP_EQUAL("MD5", GetParameter(ps, (char *)"algorithm"));
    STRCMP_EQUAL("\"asterisk\"", GetParameter(ps, (char *)"realm"));
    STRCMP_EQUAL("\"40062bed\"", GetParameter(ps, (char *)"nonce"));
    STRCMP_EQUAL("\"sip:192.168.10.62\"", GetParameter(ps, (char *)"uri"));
    STRCMP_EQUAL("\"ff880a705d5848ea0b81bdfbce0ea782\"", GetParameter(ps, (char *)"response"));

    DestroyAuthHeader((struct Header *)authHeader);
}

TEST(AuthorizationTestGroup, AuthorizationHeader2StringTest)
{
    char result[512] = {0};
    struct AuthHeader *authHeader = (struct AuthHeader *)ParseAuthHeader(AuthorizationString);
    AuthHeader2String(result, (struct Header *)authHeader);

    STRCMP_EQUAL(AuthorizationString, result);
    DestroyAuthHeader((struct Header *)authHeader);
}

TEST(AuthorizationTestGroup, SetSchemeTest)
{
    struct AuthHeader *h  = CreateAuthorizationHeader();

    AuthHeaderSetScheme(h, DIGEST);
    CHECK_EQUAL(DIGEST, AuthHeaderGetScheme(h));

    AuthHeaderSetScheme(h, BASIC);
    CHECK_EQUAL(BASIC, AuthHeaderGetScheme(h));
    

    DestroyAuthHeader((struct Header *) h);
}

TEST(AuthorizationTestGroup, ParametersAlgorithmTest)
{
    struct AuthHeader *h  = CreateAuthorizationHeader();

    AuthHeaderSetParameter(h, AUTH_HEADER_ALGORITHM, ALGORITHM_MD5);
    STRCMP_EQUAL(ALGORITHM_MD5, AuthHeaderGetParameter(h, AUTH_HEADER_ALGORITHM));
    
    DestroyAuthHeader((struct Header *) h);
}

TEST(AuthorizationTestGroup, ParametersUriTest)
{
    struct AuthHeader *h  = CreateAuthorizationHeader();

    AuthHeaderSetParameter(h, AUTH_HEADER_URI, "sip://192.168.10.62");
    STRCMP_EQUAL("\"sip://192.168.10.62\"", AuthHeaderGetParameter(h, AUTH_HEADER_URI));
    
    DestroyAuthHeader((struct Header *) h);
}

TEST(AuthorizationTestGroup, ParametersUserNameTest)
{
    struct AuthHeader *h  = CreateAuthorizationHeader();

    AuthHeaderSetParameter(h, AUTH_HEADER_USER_NAME, "88004");
    STRCMP_EQUAL("\"88004\"", AuthHeaderGetParameter(h, AUTH_HEADER_USER_NAME));
    
    DestroyAuthHeader((struct Header *) h);
}

TEST(AuthorizationTestGroup, ParametersRealmTest)
{
    struct AuthHeader *h  = CreateAuthorizationHeader();

    AuthHeaderSetParameter(h, AUTH_HEADER_REALM, "asterisk");
    STRCMP_EQUAL("\"asterisk\"", AuthHeaderGetParameter(h, AUTH_HEADER_REALM));
    
    DestroyAuthHeader((struct Header *) h);
}

TEST(AuthorizationTestGroup, ParametersResponseTest)
{
    struct AuthHeader *h  = CreateAuthorizationHeader();

    AuthHeaderSetParameter(h, AUTH_HEADER_RESPONSE, "ff880a705d5848ea0b81bdfbce0ea782");
    STRCMP_EQUAL("\"ff880a705d5848ea0b81bdfbce0ea782\"", AuthHeaderGetParameter(h, AUTH_HEADER_RESPONSE));
    
    DestroyAuthHeader((struct Header *) h);
}

TEST(AuthorizationTestGroup, ParametersNonceTest)
{
    struct AuthHeader *h  = CreateAuthorizationHeader();

    AuthHeaderSetParameter(h, AUTH_HEADER_NONCE, "ff880a705");
    STRCMP_EQUAL("\"ff880a705\"", AuthHeaderGetParameter(h, AUTH_HEADER_NONCE));
    
    DestroyAuthHeader((struct Header *) h);
}


TEST(AuthorizationTestGroup, SetParameterQuotedTest)
{
    struct AuthHeader *h  = CreateAuthorizationHeader();

    AuthHeaderSetParameter(h, AUTH_HEADER_NONCE, "\"ff880a705\"");
    STRCMP_EQUAL("\"ff880a705\"", AuthHeaderGetParameter(h, AUTH_HEADER_NONCE));
    
    DestroyAuthHeader((struct Header *) h);

}

TEST(AuthorizationTestGroup, AllParametersTest)
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

TEST(AuthorizationTestGroup, CalculateResponaseTest)
{
    char username[] = "88004";
    char passwd[] = "88004";
    char uri[] = "\"sip:192.168.10.62\"";
    char realm[] = "\"asterisk\"";
    char nonce[] = "\"40062bed\"";
    char response[64] = {0};
    
    CalculateResponse(username, passwd, uri, realm, nonce, response);
    STRCMP_EQUAL("ff880a705d5848ea0b81bdfbce0ea782", response);
}
