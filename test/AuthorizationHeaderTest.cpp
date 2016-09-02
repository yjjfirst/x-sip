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
