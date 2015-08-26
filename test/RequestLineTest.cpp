#include "CppUTest/TestHarness.h"

extern "C" {
#include <stdio.h>
#include <string.h>
#include "RequestLine.h"
}

TEST_GROUP(RequestLineTestGroup)
{
    char RegisterRequestLineString[128];
    char InviteRequestLineString[128];

    struct RequestLine *RegisterRequestLine;
    struct RequestLine *InviteRequestLine;
  

    void setup()
    {
        RegisterRequestLine = CreateRequestLine();
        InviteRequestLine = CreateRequestLine();
        
        strcpy(RegisterRequestLineString, "REGISTER sip:192.168.2.89 SIP/2.0");
        Parse(RegisterRequestLineString, RegisterRequestLine, GetRequestLinePattern());

        strcpy(InviteRequestLineString, "INVITE sip:7170@iptel.org SIP/1.0");
        Parse(InviteRequestLineString, InviteRequestLine, GetRequestLinePattern());
    }
};

TEST(RequestLineTestGroup, RegisterMethodTest)
{
    STRCMP_EQUAL("REGISTER",RequestLineGetMethod(RegisterRequestLine));
    STRCMP_EQUAL("INVITE", RequestLineGetMethod(InviteRequestLine));
}

TEST(RequestLineTestGroup, RegisterSIP_VersionTest)
{
    STRCMP_EQUAL("SIP/2.0", RequestLineGetSipVersion(RegisterRequestLine));
    STRCMP_EQUAL("SIP/1.0", RequestLineGetSipVersion(InviteRequestLine));
}

TEST(RequestLineTestGroup, RegisterURIParseTest)
{
    URI *uri = RequestLineGetUri(RegisterRequestLine);

    STRCMP_EQUAL("sip", UriGetScheme(uri));
    STRCMP_EQUAL("192.168.2.89", UriGetHost(uri));
}
