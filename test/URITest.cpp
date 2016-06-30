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
        DestroyUri(&uri);
    }
};

TEST(URITestGroup, URISchemeSIPParseTest)
{
    char URIString[] = "sip:alice:secretword@atlanta.com;transport=tcp?subject=project";
 
    ParseUri((char *)URIString, &uri);
    STRCMP_EQUAL("sip", UriGetScheme(uri));
}

TEST(URITestGroup, URISchemeSIPSParseTest)
{
    char URIString[] = "sips:peter@192.168.10.62:5060";

    ParseUri((char *)URIString, &uri);
    STRCMP_EQUAL("sips", UriGetScheme(uri));
}

TEST(URITestGroup, URIUserParseTest)
{
    char URIString[] = "sips:peter@192.168.10.62:5060";

    ParseUri((char *)URIString, &uri);
    STRCMP_EQUAL("peter", UriGetUser(uri));
}

TEST(URITestGroup, URIEmptyUserParseTest)
{
    char URIString[] = "sips:192.168.10.62:5060";

    ParseUri((char *)URIString, &uri);
    STRCMP_EQUAL("", UriGetUser(uri));
    STRCMP_EQUAL("192.168.10.62", UriGetHost(uri));
    CHECK_EQUAL(5060, UriGetPort(uri));
}

TEST(URITestGroup, URIHostParseTest)
{
    char URIString[] = "sips:peter@192.168.10.62:5060";
    ParseUri((char *)URIString, &uri);
    STRCMP_EQUAL("192.168.10.62", UriGetHost(uri));
    STRCMP_EQUAL("peter", UriGetUser(uri));
}

TEST(URITestGroup, URIHostParseNoPortTest)
{
    char URIString[] = "sips:peter@192.168.10.62";
    ParseUri((char *)URIString, &uri);
    STRCMP_EQUAL("192.168.10.62", UriGetHost(uri));

}

TEST(URITestGroup, URIPortParseTest)
{
    char URIString[] = "sips:peter@192.168.10.62:5060;transport=tcp?subject=project";
    ParseUri((char *)URIString, &uri);
    CHECK_EQUAL(5060, UriGetPort(uri));
}

TEST(URITestGroup, URIParameterParseTest)
{
    char URIString[] = "sip:alice:secretword@atlanta.com:5060;transport=tcp?subject=project";
 
    ParseUri((char *)URIString, &uri);
    STRCMP_EQUAL("tcp", UriGetParameter(uri, (char *)"transport"));
}

TEST(URITestGroup, URIParseTest)
{
    char URIString[] = "sip:alice:secretword@atlanta.com;transport=tcp?subject=project";
 
    ParseUri((char *)URIString, &uri);
    STRCMP_EQUAL("project", UriGetHeader(uri, (char *)"subject"));
    STRCMP_EQUAL("tcp", UriGetParameter(uri, (char *)"transport"));
    STRCMP_EQUAL("project", UriGetHeader(uri, (char *)"subject"));
}

TEST(URITestGroup, URIParseNoHeaderTest)
{
    char URIString[] = "sip:alice:secretword@atlanta.com;transport=tcp";
 
    ParseUri((char *)URIString, &uri);
    STRCMP_EQUAL("tcp", UriGetParameter(uri, (char *)"transport"));
    STRCMP_EQUAL("sip", UriGetScheme(uri));
    STRCMP_EQUAL("alice:secretword", UriGetUser(uri));
    STRCMP_EQUAL("atlanta.com", UriGetHost(uri));

}

TEST(URITestGroup, URIParseNoParameterTest)
{
    char URIString[] = "sip:alice:secretword@atlanta.com?subject=project";
 
    ParseUri((char *)URIString, &uri);
    STRCMP_EQUAL("sip", UriGetScheme(uri));
    STRCMP_EQUAL("project", UriGetHeader(uri, (char *)"subject"));
    STRCMP_EQUAL("alice:secretword", UriGetUser(uri));
    STRCMP_EQUAL("atlanta.com", UriGetHost(uri));
}

TEST(URITestGroup, AnotherURI2StringTest)
{
    char s[] = "sip:registrar.munich.de";
    char result[128] = {0};
    
    ParseUri((char *)s, &uri);
    Uri2StringExt(result, uri);
    
    STRCMP_EQUAL(s, result);
}

TEST(URITestGroup, URI2StringTest)
{
    char URIString[] = "sip:atlanta.com?subject=project";
    char result[128] = {0};

    ParseUri((char *)URIString, &uri);

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

    ParseUri((char *)URIString, &uri);
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

TEST(URITestGroup, URIMatchedTest)
{
    struct URI *uri2 = CreateEmptyUri();
    char URIString[] = "sip:alice:secretword@atlanta.com;transport=tcp?subject=project";
 
    ParseUri((char *)URIString, &uri2);
    ParseUri((char *)URIString, &uri);

    CHECK_TRUE(UriMatched(uri, uri2));

    DestroyUri(&uri2);
}

TEST(URITestGroup, URIPortUnmatchTest)
{
    struct URI *uri2 = CreateEmptyUri();
    char URIString[] = "sip:alice:secretword@atlanta.com;transport=tcp?subject=project";
    char URIString2[] = "sip:alice:secretword@atlanta.com:5060;transport=tcp?subject=project";
    ParseUri((char *)URIString2, &uri2);
    ParseUri((char *)URIString, &uri);

    CHECK_FALSE(UriMatched(uri, uri2));

    DestroyUri(&uri2);
}

TEST(URITestGroup, URISchemeUnmatchTest)
{
    struct URI *uri2 = CreateEmptyUri();
    char URIString[] = "sip:alice:secretword@atlanta.com;transport=tcp?subject=project";
    char URIString2[] = "sips:alice:secretword@atlanta.com;transport=tcp?subject=project";
    ParseUri((char *)URIString2, &uri2);
    ParseUri((char *)URIString, &uri);

    CHECK_FALSE(UriMatched(uri, uri2));

    DestroyUri(&uri2);
}

TEST(URITestGroup, URIUserUnmatchTest)
{
    struct URI *uri2 = CreateEmptyUri();
    char URIString[] = "sip:alice:secretword@atlanta.com;transport=tcp?subject=project";
    char URIString2[] = "sip:Peter:secretword@atlanta.com;transport=tcp?subject=project";
    ParseUri((char *)URIString2, &uri2);
    ParseUri((char *)URIString, &uri);

    CHECK_FALSE(UriMatched(uri, uri2));

    DestroyUri(&uri2);
}

TEST(URITestGroup, URIHostUnmatchTest)
{
    struct URI *uri2 = CreateEmptyUri();
    char URIString[] = "sip:alice:secretword@atlanta.com;transport=tcp?subject=project";
    char URIString2[] = "sip:alice:secretword@delta.com;transport=tcp?subject=project";
    ParseUri((char *)URIString2, &uri2);
    ParseUri((char *)URIString, &uri);

    CHECK_FALSE(UriMatched(uri, uri2));

    DestroyUri(&uri2);
}

TEST(URITestGroup, URIParametersUnmatchTest)
{
    struct URI *uri2 = CreateEmptyUri();
    char URIString[] = "sip:alice:secretword@atlanta.com;transport=tcp?subject=project";
    char URIString2[] = "sip:alice:secretword@atlanta.com;transport=udp?subject=project";
    ParseUri((char *)URIString2, &uri2);
    ParseUri((char *)URIString, &uri);

    CHECK_FALSE(UriMatched(uri, uri2));

    DestroyUri(&uri2);
}

TEST(URITestGroup, URIHeadersUnmatchTest)
{
    struct URI *uri2 = CreateEmptyUri();
    char URIString[] = "sip:alice:secretword@atlanta.com;transport=tcp?subject=project";
    char URIString2[] = "sip:alice:secretword@atlanta.com;transport=udp?subject=management";
    ParseUri((char *)URIString2, &uri2);
    ParseUri((char *)URIString, &uri);

    CHECK_FALSE(UriMatched(uri, uri2));

    DestroyUri(&uri2);
}

TEST(URITestGroup, URIDup)
{
    char URIString[] = "sip:alice:secretword@atlanta.com;transport=tcp?subject=project";

    ParseUri((char *)URIString, &uri);    
    struct URI *dest = UriDup(uri);
    CHECK_TRUE(UriMatched(uri, dest));

    DestroyUri(&dest);
}

TEST(URITestGroup, URIDupNull)
{
    struct URI *src = NULL;
    
    POINTERS_EQUAL(NULL, UriDup(src));

}










