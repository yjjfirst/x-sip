#include "CppUTest/TestHarness.h"
#include "URI.h"

extern "C" {

#include <string.h>
}

TEST_GROUP(URITestGroup)
{
    char URIString[128];
    void setup()
    {
        strcpy(URIString, "sip:alice:secretword@atlanta.com;transport=tcp");
    }
};

TEST(URITestGroup, URISchemeParseTest)
{
    struct URI uri;
    ParseURI(URIString, &uri);

    STRCMP_EQUAL("sip", uri.scheme);
    STRCMP_EQUAL("alice", uri.user);
    STRCMP_EQUAL("secretword", uri.password);
}
