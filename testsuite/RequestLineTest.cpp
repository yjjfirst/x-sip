#include "CppUTest/TestHarness.h"

extern "C" {
#include "RequestLine.h"
#include "string.h"
}

TEST_GROUP(RequestLineTestGroup)
{
    char requestLineString[128];
    struct RequestLine requestLine;

    void setup()
    {
        strcpy(requestLineString, "REGISTER sip:192.168.2.89 SIP/2.0");
        parseRequestLine(requestLineString, &requestLine);
    }
};

TEST(RequestLineTestGroup, MethodTest)
{
    STRCMP_EQUAL(requestLine.Method, "REGISTER");
}

TEST(RequestLineTestGroup, Request_URITest)
{
    STRCMP_EQUAL(requestLine.Request_URI, "sip:192.168.2.89");
}

TEST(RequestLineTestGroup, SIP_VersionTest)
{
    STRCMP_EQUAL(requestLine.SIP_Version, "SIP/2.0");
}
