#include "CppUTest/TestHarness.h"

extern "C" {
#include <stdio.h>
#include <string.h>

#include "RequestLine.h"
#include "Parser.h"
}

TEST_GROUP(RequestLineTestGroup)
{
    char RegisterRequestLineString[128];
    char InviteRequestLineString[128];

    struct RequestLine *RegisterRequestLine;
    struct RequestLine *InviteRequestLine;
  

    void setup()
    {
        RegisterRequestLine = CreateEmptyRequestLine();
        InviteRequestLine = CreateEmptyRequestLine();
        
        strcpy(RegisterRequestLineString, "REGISTER sip:192.168.2.89 SIP/2.0");
        Parse(RegisterRequestLineString, RegisterRequestLine, GetRequestLinePattern());

        strcpy(InviteRequestLineString, "INVITE sip:7170@iptel.org SIP/1.0");
        Parse(InviteRequestLineString, InviteRequestLine, GetRequestLinePattern());
    }

    void teardown()
    {
        DestoryRequestLine(RegisterRequestLine);
        DestoryRequestLine(InviteRequestLine);
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

TEST(RequestLineTestGroup, LongMethodParseTest)
{
    char string[] = "REGISTER90123456789 sip:192.168.2.89 SIP/2.0";
    struct RequestLine *r = CreateEmptyRequestLine();

    CHECK_EQUAL(-1, Parse(string, r, GetRequestLinePattern()));
    DestoryRequestLine(r);
}

TEST(RequestLineTestGroup, CreateRequestLineTest)
{
    struct RequestLine *r = CreateEmptyRequestLine();

    RequestLineSetMethod(r, (char *)"REGISTER");
    STRCMP_EQUAL("REGISTER", RequestLineGetMethod(r));
    
    RequestLineSetMethod(r, (char *)"INVITE");
    STRCMP_EQUAL("INVITE", RequestLineGetMethod(r));

    DestoryRequestLine(r);
}

TEST(RequestLineTestGroup, RequestLineWrongMethodTest)
{
    struct RequestLine *r = CreateEmptyRequestLine();
    RequestLineSetMethod(r, (char *)"aaaaaaa");

    STRCMP_EQUAL("", RequestLineGetMethod(r));
    DestoryRequestLine(r);
}

TEST(RequestLineTestGroup, RequestLineSetSipVersionTest)
{
    struct RequestLine *r = CreateEmptyRequestLine();

    RequestLineSetSipVersion(r, (char *)"SIP/2.0");
    STRCMP_EQUAL("SIP/2.0", RequestLineGetSipVersion(r));
    DestoryRequestLine(r);
}

TEST(RequestLineTestGroup, RequestLineSetUriTest)
{
    struct RequestLine *r = CreateEmptyRequestLine();
    struct URI *u = CreateUri();
    char URIString[] = "sips:peter@192.168.10.62:5060";
    
    Parse((char *)URIString, u, GetURIHeaderPattern(URIString));
    RequestLineSetUri(r, u);

    DestoryRequestLine(r);
}

TEST(RequestLineTestGroup, RequestLine2StringTest)
{
    struct RequestLine *r = CreateEmptyRequestLine();
    struct URI *u = CreateUri();
    char string[256] = {0};
    
    RequestLineSetMethod(r, (char *)"INVITE");
    RequestLineSetSipVersion(r, (char *)"SIP/2.0");

    char URIString[] = "sips:192.168.10.62:5060";    
    Parse((char *)URIString, u, GetURIHeaderPattern(URIString));
    RequestLineSetUri(r, u);
    STRCMP_EQUAL("", UriGetUser(u));
    STRCMP_EQUAL("192.168.10.62", UriGetHost(u));
    STRCMP_EQUAL("5060", UriGetPort(u));
    
    RequestLine2String(string, r);
    STRCMP_EQUAL("INVITE sips:192.168.10.62:5060 SIP/2.0", string);

    DestoryRequestLine(r);
}

