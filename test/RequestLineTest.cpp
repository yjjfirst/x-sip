#include "CppUTest/TestHarness.h"

extern "C" {
#include <string.h>
#include "RequestLine.h"
}

TEST_GROUP(RequestLineTestGroup)
{
    char RegisterRequestLineString[128];
    char InviteRequestLineString[128];

    struct RequestLine RegisterRequestLine;
    struct RequestLine InviteRequestLine;
  

    void setup()
    {
        
        strcpy(RegisterRequestLineString, "REGISTER sip:192.168.2.89 SIP/2.0");
        Parse(RegisterRequestLineString, &RegisterRequestLine,GetRequestLinePattern());

        strcpy(InviteRequestLineString, "INVITE sip:7170@iptel.org SIP/1.0");
        Parse(InviteRequestLineString, &InviteRequestLine,GetRequestLinePattern());
        
    }
};

TEST(RequestLineTestGroup, RegisterMethodTest)
{
    STRCMP_EQUAL("REGISTER",RegisterRequestLine.Method);
    STRCMP_EQUAL("INVITE", InviteRequestLine.Method);
}

TEST(RequestLineTestGroup, RegisterRequest_URITest)
{
    STRCMP_EQUAL(RegisterRequestLine.Request_URI, "sip:192.168.2.89");
    STRCMP_EQUAL(InviteRequestLine.Request_URI, "sip:7170@iptel.org");
}

TEST(RequestLineTestGroup, RegisterSIP_VersionTest)
{
    STRCMP_EQUAL("SIP/2.0", RegisterRequestLine.SIP_Version);
    STRCMP_EQUAL("SIP/1.0", InviteRequestLine.SIP_Version);
}
