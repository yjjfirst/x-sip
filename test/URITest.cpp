#include "CppUTest/TestHarness.h"

extern "C" {
#include <stdio.h>
#include <string.h>
#include "URI.h"
#include "RequestLine.h"

}

TEST_GROUP(URITestGroup)
{
    void setup()
    {
    }
};

TEST(URITestGroup, URISchemeParseTest)
{
    struct URI uri;
    char *URIString1 = "sip:alice:secretword@atlanta.com;transport=tcp?subject=project";
    char *URIString2 = "sips:peter@192.168.10.62:5060";

    ParseURI(URIString1, &uri);
    STRCMP_EQUAL("sip", uri.scheme);
    STRCMP_EQUAL("alice", uri.user);

    ParseURI(URIString2, &uri);

    STRCMP_EQUAL("sips", uri.scheme);
    STRCMP_EQUAL("peter", uri.user);

}
