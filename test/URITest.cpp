#include "CppUTest/TestHarness.h"

extern "C" {
#include <stdio.h>
#include <string.h>

#include "URI.h"
#include "Parser.h"
#include "Parameter.h"
}

TEST_GROUP(URITestGroup)
{
    struct URI *uri;
    void setup()
    {
        uri = CreateEmptyUri();
    }

    void teardown()
    {
        DestoryUri(uri);
    }
};

TEST(URITestGroup, URISchemeSIPParseTest)
{
    char URIString[] = "sip:alice:secretword@atlanta.com;transport=tcp?subject=project";
 
    ParseURI((char *)URIString, &uri);
    STRCMP_EQUAL("sip", UriGetScheme(uri));
}

TEST(URITestGroup, URISchemeSIPSParseTest)
{
    char URIString[] = "sips:peter@192.168.10.62:5060";

    ParseURI((char *)URIString, &uri);
    STRCMP_EQUAL("sips", UriGetScheme(uri));
}

TEST(URITestGroup, URIUserParseTest)
{
    char URIString[] = "sips:peter@192.168.10.62:5060";

    ParseURI((char *)URIString, &uri);
    STRCMP_EQUAL("peter", UriGetUser(uri));
}

TEST(URITestGroup, URIEmptyUserParseTest)
{
    char URIString[] = "sips:192.168.10.62:5060";

    ParseURI((char *)URIString, &uri);
    STRCMP_EQUAL("", UriGetUser(uri));
    STRCMP_EQUAL("192.168.10.62", UriGetHost(uri));
    CHECK_EQUAL(5060, UriGetPort(uri));
}

TEST(URITestGroup, URIHostParseTest)
{
    char URIString[] = "sips:peter@192.168.10.62:5060";
    ParseURI((char *)URIString, &uri);
    STRCMP_EQUAL("192.168.10.62", UriGetHost(uri));
    STRCMP_EQUAL("peter", UriGetUser(uri));
}

TEST(URITestGroup, URIHostParseNoPortTest)
{
    char URIString[] = "sips:peter@192.168.10.62";
    ParseURI((char *)URIString, &uri);
    STRCMP_EQUAL("192.168.10.62", UriGetHost(uri));

}

TEST(URITestGroup, URIPortParseTest)
{
    char URIString[] = "sips:peter@192.168.10.62:5060;transport=tcp?subject=project";
    ParseURI((char *)URIString, &uri);
    CHECK_EQUAL(5060, UriGetPort(uri));
}

TEST(URITestGroup, URIParameterParseTest)
{
    char URIString[] = "sip:alice:secretword@atlanta.com:5060;transport=tcp?subject=project";
 
    ParseURI((char *)URIString, &uri);
    STRCMP_EQUAL("tcp", UriGetParameter(uri, (char *)"transport"));
}

TEST(URITestGroup, URIParseTest)
{
    char URIString[] = "sip:alice:secretword@atlanta.com;transport=tcp?subject=project";
 
    ParseURI((char *)URIString, &uri);
    STRCMP_EQUAL("project", UriGetHeader(uri, (char *)"subject"));
    STRCMP_EQUAL("tcp", UriGetParameter(uri, (char *)"transport"));
    STRCMP_EQUAL("project", UriGetHeader(uri, (char *)"subject"));
}

TEST(URITestGroup, URIParseNoHeaderTest)
{
    char URIString[] = "sip:alice:secretword@atlanta.com;transport=tcp";
 
    ParseURI((char *)URIString, &uri);
    STRCMP_EQUAL("tcp", UriGetParameter(uri, (char *)"transport"));
    STRCMP_EQUAL("sip", UriGetScheme(uri));
    STRCMP_EQUAL("alice:secretword", UriGetUser(uri));
    STRCMP_EQUAL("atlanta.com", UriGetHost(uri));

}

TEST(URITestGroup, URIParseNoParameterTest)
{
    char URIString[] = "sip:alice:secretword@atlanta.com?subject=project";
 
    ParseURI((char *)URIString, &uri);
    STRCMP_EQUAL("sip", UriGetScheme(uri));
    STRCMP_EQUAL("project", UriGetHeader(uri, (char *)"subject"));
    STRCMP_EQUAL("alice:secretword", UriGetUser(uri));
    STRCMP_EQUAL("atlanta.com", UriGetHost(uri));
}

TEST(URITestGroup, AnotherURI2StringTest)
{
    char s[] = "sip:registrar.munich.de";
    char result[128] = {0};
    
    ParseURI((char *)s, &uri);
    Uri2StringExt(result, uri);
    
    STRCMP_EQUAL(s, result);
}

TEST(URITestGroup, URI2StringTest)
{
    char URIString[] = "sip:atlanta.com?subject=project";
    char result[128] = {0};

    ParseURI((char *)URIString, &uri);

    Uri2StringExt(result, uri);
    STRCMP_EQUAL("sip", UriGetScheme(uri));
    STRCMP_EQUAL("", UriGetUser(uri));
    STRCMP_EQUAL("atlanta.com",UriGetHost(uri));
    STRCMP_EQUAL("project",UriGetHeader(uri, (char *)"subject"));
    STRCMP_EQUAL(URIString, result);
}

TEST(URITestGroup, URINoUserParseTest)
{
    char URIString[] = "sip:registrar.munich.de";

    ParseURI((char *)URIString, &uri);
    STRCMP_EQUAL("", UriGetUser(uri));
}

TEST(URITestGroup, URIFieldSetTest)
{
    UriSetScheme(uri, (char *)"sips");
    UriSetUser(uri, (char *)"Martin");
    UriSetHost(uri, (char *)"192.168.10.62");

    struct Parameters *hs = CreateParameters();
    AddParameter(hs, (char *)"subject", (char *)"project");
    UriSetHeaders(uri, hs);
    
    struct Parameters *ps = CreateParameters();
    ParseParametersExt((char *)"transport=udp", ps);
    UriSetParameters(uri, ps);

    UriSetPort(uri, 5061);

    STRCMP_EQUAL("sips", UriGetScheme(uri));
    STRCMP_EQUAL("Martin", UriGetUser(uri));
    STRCMP_EQUAL("192.168.10.62", UriGetHost(uri));
    STRCMP_EQUAL("udp", UriGetParameter(uri,(char *)"transport"));
    STRCMP_EQUAL("project", UriGetHeader(uri,(char *)"subject"));
    CHECK_EQUAL(5061, UriGetPort(uri));
}
