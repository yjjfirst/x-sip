#include "CppUTest/TestHarness.h"

extern "C" {
#include <stdio.h>
#include <string.h>

#include "URI.h"
#include "Parser.h"

}

TEST_GROUP(URITestGroup)
{
    void setup()
    {
    }
};

TEST(URITestGroup, URISchemeSIPParseTest)
{
    struct URI uri;
    char *URIString = "sip:alice:secretword@atlanta.com;transport=tcp?subject=project";
 
    Parse(URIString, &uri, GetURIParsePattern());
    STRCMP_EQUAL("sip", uri.scheme);
}

TEST(URITestGroup, URISchemeSIPSParseTest)
{
    struct URI uri;
    char *URIString = "sips:peter@192.168.10.62:5060";

    Parse(URIString, &uri, GetURIParsePattern());
    STRCMP_EQUAL("sips", uri.scheme);
}
