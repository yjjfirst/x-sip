#include "CppUTest/TestHarness.h"

extern "C" {
#include "Parser.h"
#include "ContentLength.h"
};

TEST_GROUP(ContentLengthHeaderTestGroup)
{
};

TEST(ContentLengthHeaderTestGroup, ContentLengthHeaderParseTest)
{
    struct ContentLengthHeader *c = CreateContentLengthHeader();
    char header[] = "Content-Length : 1024";

    Parse(header, c, GetContentLengthHeaderPattern());
    STRCMP_EQUAL("Content-Length", ContentLengthGetName(c));
    CHECK_EQUAL(1024, ContentLengthGetLength(c));
    DestoryContentLengthHeader((struct Header *)c);
}
