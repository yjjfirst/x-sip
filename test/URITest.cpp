#include "CppUTest/TestHarness.h"

extern "C" {
#include <stdio.h>
#include <string.h>

#include "URI.h"
#include "Parser.h"

}

TEST_GROUP(URITestGroup)
{
    struct URI *uri;
    void setup()
    {
        uri = CreateUri();
    }

    void teardown()
    {
        DestoryUri(uri);
    }
};

TEST(URITestGroup, URISchemeSIPParseTest)
{
    char URIString[] = "sip:alice:secretword@atlanta.com;transport=tcp?subject=project";
 
    Parse((char *)URIString, uri, GetURIHeaderPattern(URIString));
    STRCMP_EQUAL("sip", UriGetScheme(uri));
}

TEST(URITestGroup, URISchemeSIPSParseTest)
{
    char URIString[] = "sips:peter@192.168.10.62:5060";

    Parse((char *)URIString, uri, GetURIHeaderPattern(URIString));
    STRCMP_EQUAL("sips", UriGetScheme(uri));
}

TEST(URITestGroup, URIUserParseTest)
{
    char URIString[] = "sips:peter@192.168.10.62:5060";

    Parse((char *)URIString, uri, GetURIHeaderPattern(URIString));
    STRCMP_EQUAL("peter", UriGetUser(uri));
}

TEST(URITestGroup, URIEmptyUserParseTest)
{
    char URIString[] = "sips:192.168.10.62:5060";

    Parse((char *)URIString, uri, GetURIHeaderPattern(URIString));
    STRCMP_EQUAL("", UriGetUser(uri));
    STRCMP_EQUAL("192.168.10.62", UriGetHost(uri));
    STRCMP_EQUAL("5060", UriGetPort(uri));
}

TEST(URITestGroup, URIHostParseTest)
{
    char URIString[] = "sips:peter@192.168.10.62:5060";
    Parse((char *)URIString, uri, GetURIHeaderPattern(URIString));
    STRCMP_EQUAL("192.168.10.62", UriGetHost(uri));
    STRCMP_EQUAL("peter", UriGetUser(uri));
}

TEST(URITestGroup, URIHostParseNoPortTest)
{
    char URIString[] = "sips:peter@192.168.10.62";
    Parse((char *)URIString, uri, GetURIHeaderPattern(URIString));
    STRCMP_EQUAL("192.168.10.62", UriGetHost(uri));

}

TEST(URITestGroup, URIPortParseTest)
{
    char URIString[] = "sips:peter@192.168.10.62:5060;transport=tcp?subject=project";
    Parse((char *)URIString, uri, GetURIHeaderPattern(URIString));
    STRCMP_EQUAL("5060", UriGetPort(uri));
}

TEST(URITestGroup, URIParameterParseTest)
{
    char URIString[] = "sip:alice:secretword@atlanta.com:5060;transport=tcp?subject=project";
 
    Parse((char *)URIString, uri, GetURIHeaderPattern(URIString));
    STRCMP_EQUAL("transport=tcp", UriGetParameters(uri));
}

TEST(URITestGroup, URIParseTest)
{
    char URIString[] = "sip:alice:secretword@atlanta.com;transport=tcp?subject=project";
 
    Parse((char *)URIString, uri, GetURIHeaderPattern(URIString));
    STRCMP_EQUAL("subject=project", UriGetHeaders(uri));
    STRCMP_EQUAL("transport=tcp", UriGetParameters(uri));
    STRCMP_EQUAL("subject=project", UriGetHeaders(uri));
}

TEST(URITestGroup, URIParseNoHeaderTest)
{
    char URIString[] = "sip:alice:secretword@atlanta.com;transport=tcp";
 
    Parse((char *)URIString, uri, GetURIHeaderPattern(URIString));
    STRCMP_EQUAL("transport=tcp", UriGetParameters(uri));
    STRCMP_EQUAL("sip", UriGetScheme(uri));
    STRCMP_EQUAL("alice:secretword", UriGetUser(uri));
    STRCMP_EQUAL("atlanta.com", UriGetHost(uri));
    STRCMP_EQUAL("transport=tcp", UriGetParameters(uri));

}

TEST(URITestGroup, URIParseNoParameterTest)
{
    char URIString[] = "sip:alice:secretword@atlanta.com?subject=project";
 
    Parse((char *)URIString, uri, GetURIHeaderPattern(URIString));
    STRCMP_EQUAL("sip", UriGetScheme(uri));
    STRCMP_EQUAL("subject=project", UriGetHeaders(uri));
    STRCMP_EQUAL("alice:secretword", UriGetUser(uri));
    STRCMP_EQUAL("atlanta.com", UriGetHost(uri));
}
TEST(URITestGroup, AnotherURI2StringTest)
{
    char s[] = "sip:registrar.munich.de";
    char result[128] = {0};
    
    Parse((char *)s, uri, GetURIHeaderPattern(s));
    Uri2StringExt(result, uri, GetURIHeaderPattern(s));
    
    STRCMP_EQUAL(s, result);
}

TEST(URITestGroup, URI2StringTest)
{
    char URIString[] = "sip:atlanta.com?subject=project";
    char result[128] = {0};

    Parse((char *)URIString, uri, GetURIHeaderPattern(URIString));

    Uri2StringExt(result, uri, GetURIHeaderPattern(URIString));
    STRCMP_EQUAL("sip", UriGetScheme(uri));
    STRCMP_EQUAL("", UriGetUser(uri));
    STRCMP_EQUAL("atlanta.com",UriGetHost(uri));
    STRCMP_EQUAL("subject=project",UriGetHeaders(uri));
    STRCMP_EQUAL(URIString, result);
}


TEST(URITestGroup, URINoUserParsetest)
{
    char URIString[] = "sip:registrar.munich.de";

    Parse((char *)URIString, uri, GetURIHeaderPattern(URIString));
    STRCMP_EQUAL("", UriGetUser(uri));
}

