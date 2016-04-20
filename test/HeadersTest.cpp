#include "CppUTest/TestHarness.h"

extern "C" {
#include "Headers.h"
}

TEST_GROUP(HeadersTestGroup)
{
    struct Headers *headers;

    void setup() {
        headers = CreateHeaders();
    }

    void teardown() {
        DestoryHeaders(&headers);
    }
};

TEST(HeadersTestGroup, CreateHeadersTest)
{
    CHECK_FALSE(NULL == headers);
    CHECK_EQUAL(0, HeadersLength(headers));
}

TEST(HeadersTestGroup, AddOneHeaderTest)
{
}
