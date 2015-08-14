#include "CppUTest/TestHarness.h"

extern "C" {
#include <stdio.h>
#include <string.h>

#include "URI.h"
#include "Parser.h"

}

TEST_GROUP(URITestGroup)
{
    struct URI uri;
    void setup()
    {
        bzero((void*)&uri, sizeof(uri));
    }
};

TEST(URITestGroup, URISchemeSIPParseTest)
{
    const char *URIString = "sip:alice:secretword@atlanta.com;transport=tcp?subject=project";
 
    Parse((char *)URIString, &uri, GetURIParsePattern());
    STRCMP_EQUAL("sip", uri.scheme);
}

TEST(URITestGroup, URISchemeSIPSParseTest)
{
    const char *URIString = "sips:peter@192.168.10.62:5060";

    Parse((char *)URIString, &uri, GetURIParsePattern());
    STRCMP_EQUAL("sips", uri.scheme);
}

TEST(URITestGroup, URIUserParseTest)
{
    const char *URIString = "sips:peter@192.168.10.62:5060";

    Parse((char *)URIString, &uri, GetURIParsePattern());
    STRCMP_EQUAL("peter", uri.user);
}

TEST(URITestGroup, URIEmptyUserParseTest)
{
    const char *URIString = "sips:192.168.10.62:5060";

    strcpy(uri.user, "abc");
    Parse((char *)URIString, &uri, GetURIParsePattern());
    STRCMP_EQUAL("", uri.user);
}

TEST(URITestGroup, URIHostParseTest)
{
    const char *URIString = "sips:peter@192.168.10.62:5060";
    Parse((char *)URIString, &uri, GetURIParsePattern());
    STRCMP_EQUAL("192.168.10.62", uri.host);
}

TEST(URITestGroup, URIHostParseNoPortTest)
{
    const char *URIString = "sips:peter@192.168.10.62";
    Parse((char *)URIString, &uri, GetURIParsePattern());
    STRCMP_EQUAL("192.168.10.62", uri.host);

}

TEST(URITestGroup, URIPortParseTest)
{
    const char *URIString = "sips:peter@192.168.10.62:5060;transport=tcp?subject=project";
    Parse((char *)URIString, &uri, GetURIParsePattern());
    STRCMP_EQUAL("5060", uri.port);

}

TEST(URITestGroup, URIParameterParseTest)
{
    const char *URIString = "sip:alice:secretword@atlanta.com:5060;transport=tcp?subject=project";
 
    Parse((char *)URIString, &uri, GetURIParsePattern());
    STRCMP_EQUAL("transport=tcp", uri.parameters);
}

TEST(URITestGroup, URIHeaderParseTest)
{
    const char *URIString = "sip:alice:secretword@atlanta.com;transport=tcp?subject=project";
 
    Parse((char *)URIString, &uri, GetURIParsePattern());
    STRCMP_EQUAL("subject=project", uri.headers);
}

TEST(URITestGroup, URIParameterParseNoHeaderTest)
{
    const char *URIString = "sip:alice:secretword@atlanta.com;transport=tcp";
 
    Parse((char *)URIString, &uri, GetURIParsePattern());
    STRCMP_EQUAL("transport=tcp", uri.parameters);

}

TEST(URITestGroup, URIHeaderParseNoParameterTest)
{
    const char *URIString = "sip:alice:secretword@atlanta.com?subject=project";
 
    Parse((char *)URIString, &uri, GetURIParsePattern());
    STRCMP_EQUAL("subject=project", uri.headers);

}
