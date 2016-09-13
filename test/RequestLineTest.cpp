#include "CppUTest/TestHarness.h"

extern "C" {
#include <stdio.h>
#include <string.h>

#include "URI.h"
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

        ParseRequestLine(RegisterRequestLineString, RegisterRequestLine);

        strcpy(InviteRequestLineString, "INVITE sip:7170@iptel.org SIP/1.0");
        ParseRequestLine(InviteRequestLineString, InviteRequestLine);
    }

    void teardown()
    {
        DestroyRequestLine(RegisterRequestLine);
        DestroyRequestLine(InviteRequestLine);
    }
};

TEST(RequestLineTestGroup, RegisterMethodTest)
{
    STRCMP_EQUAL("REGISTER",RequestLineGetMethodName(RegisterRequestLine));
    STRCMP_EQUAL("INVITE", RequestLineGetMethodName(InviteRequestLine));
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

    CHECK_EQUAL(-1, ParseRequestLine(string, r));
    DestroyRequestLine(r);
}

TEST(RequestLineTestGroup, CreateRequestLineTest)
{
    struct RequestLine *r = CreateEmptyRequestLine();

    RequestLineSetMethod(r, (char *)"REGISTER");
    STRCMP_EQUAL("REGISTER", RequestLineGetMethodName(r));
    
    RequestLineSetMethod(r, (char *)"INVITE");
    STRCMP_EQUAL("INVITE", RequestLineGetMethodName(r));

    DestroyRequestLine(r);
}

TEST(RequestLineTestGroup, RequestLineWrongMethodTest)
{
    struct RequestLine *r = CreateEmptyRequestLine();
    RequestLineSetMethod(r, (char *)"aaaaaaa");

    STRCMP_EQUAL("", RequestLineGetMethodName(r));
    DestroyRequestLine(r);
}

TEST(RequestLineTestGroup, RequestLineSetSipVersionTest)
{
    struct RequestLine *r = CreateEmptyRequestLine();

    RequestLineSetSipVersion(r, (char *)"SIP/2.0");
    STRCMP_EQUAL("SIP/2.0", RequestLineGetSipVersion(r));
    DestroyRequestLine(r);
}

TEST(RequestLineTestGroup, RequestLineSetUriTest)
{
    struct RequestLine *r = CreateEmptyRequestLine();
    URI *u = CreateEmptyUri();
    char URIString[] = "sips:peter@192.168.10.62:5060";
    
    ParseUri((char *)URIString, &u);
    RequestLineSetUri(r, u);

    u = RequestLineGetUri(r);
    STRCMP_EQUAL("sips", UriGetScheme(u));
    STRCMP_EQUAL("peter", UriGetUser(u));
    STRCMP_EQUAL("192.168.10.62", UriGetHost(u));
    CHECK_EQUAL(5060, UriGetPort(u));
    DestroyRequestLine(r);
}

TEST(RequestLineTestGroup, RequestLine2StringTest)
{
    struct RequestLine *r = CreateEmptyRequestLine();
    URI *u = CreateEmptyUri();
    char string[256] = {0};
    
    RequestLineSetMethod(r, (char *)"INVITE");
    RequestLineSetSipVersion(r, (char *)"SIP/2.0");

    char URIString[] = "sips:192.168.10.62:5060";    
    ParseUri((char *)URIString, &u);
    RequestLineSetUri(r, u);
    STRCMP_EQUAL("", UriGetUser(u));
    STRCMP_EQUAL("192.168.10.62", UriGetHost(u));
    CHECK_EQUAL(5060, UriGetPort(u));
    
    RequestLine2String(string, r);
    STRCMP_EQUAL("INVITE sips:192.168.10.62:5060 SIP/2.0", string);

    DestroyRequestLine(r);
}

TEST(RequestLineTestGroup, CreateNonEmptyRequestLineTest)
{
    SIP_METHOD m = SIP_METHOD_REGISTER;
    char URIString[] = "sip:registrar.biloxi.com";
    URI *u = CreateEmptyUri();
    struct RequestLine *r = NULL;

    ParseUri((char *)URIString, &u);
    r = CreateRequestLine(m, u); 
    STRCMP_EQUAL("REGISTER", RequestLineGetMethodName(r));
    STRCMP_EQUAL("SIP/2.0", RequestLineGetSipVersion(r));
    
    u = RequestLineGetUri(r);
    STRCMP_EQUAL("sip", UriGetScheme(u));
    STRCMP_EQUAL("registrar.biloxi.com", UriGetHost(u));
    CHECK_EQUAL(0, UriGetPort(u));

    DestroyRequestLine(r);
}

TEST(RequestLineTestGroup, SetMethodTest)
{
    URI *u = CreateUri((char *)URI_SCHEME_SIP, (char *)"", (char *)"192.168.10.62", 0);
    struct RequestLine *rl = CreateRequestLine(SIP_METHOD_INVITE, u);

    STRCMP_EQUAL(SIP_METHOD_NAME_INVITE, RequestLineGetMethodName(rl));

    DestroyRequestLine(rl);
}

TEST(RequestLineTestGroup, RequestLineMatchedTest)
{
    char requestLine[] = "REGISTER sip:192.168.2.89 SIP/2.0";    
    struct RequestLine *rl = CreateEmptyRequestLine();

    ParseRequestLine(requestLine, rl);
    CHECK_TRUE(RequestLineMatched(rl, RegisterRequestLine));
    
    DestroyRequestLine(rl);
}

TEST(RequestLineTestGroup, RequestLineUriUnmatchedTest)
{
    char requestLine[] = "REGISTER sip:192.168.2.99 SIP/2.0";    
    struct RequestLine *rl = CreateEmptyRequestLine();

    ParseRequestLine(requestLine, rl);
    CHECK_FALSE(RequestLineMatched(rl, RegisterRequestLine));
    
    DestroyRequestLine(rl);
}

TEST(RequestLineTestGroup, RequestLineMethodUnmatchedTest)
{
    char requestLine[] = "INVITE sip:192.168.2.89 SIP/2.0";    
    struct RequestLine *rl = CreateEmptyRequestLine();

    ParseRequestLine(requestLine, rl);
    CHECK_FALSE(RequestLineMatched(rl, RegisterRequestLine));
    
    DestroyRequestLine(rl);
}

TEST(RequestLineTestGroup, RequestLineVersionUnmatchedTest)
{
    char requestLine[] = "REGISTER sip:192.168.2.89 SIPS/2.0";    
    struct RequestLine *rl = CreateEmptyRequestLine();

    ParseRequestLine(requestLine, rl);
    CHECK_FALSE(RequestLineMatched(rl, RegisterRequestLine));
    
    DestroyRequestLine(rl);
}

TEST(RequestLineTestGroup, RequestLineDupTest)
{
    struct RequestLine *rl = RequestLineDup(RegisterRequestLine);

    CHECK_TRUE(RequestLineMatched(RegisterRequestLine, rl));
    DestroyRequestLine(rl);
    
}

TEST(RequestLineTestGroup, RequestLineMethodMatchedTest)
{
    struct RequestLine *rl = RequestLineDup(RegisterRequestLine);

    CHECK_TRUE(RequestLineMethodMatched(RegisterRequestLine, rl));

    DestroyRequestLine(rl);
}

TEST(RequestLineTestGroup, RequestLineMethodNonMatchedTest)
{
    struct RequestLine *rl = RequestLineDup(RegisterRequestLine);
    RequestLineSetMethod(rl, (char *)SIP_METHOD_NAME_INFO);

    CHECK_FALSE(RequestLineMethodMatched(RegisterRequestLine, rl));

    DestroyRequestLine(rl);
}
