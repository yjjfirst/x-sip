#include "CppUTest/TestHarness.h"

extern "C" {
#include "Headers.h"
#include "ContactHeader.h"
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
    struct ContactHeader *to = CreateToHeader();
    HeadersAddHeader(headers, (struct Header *)to);

    CHECK_EQUAL(1, HeadersLength(headers));
    
}

TEST(HeadersTestGroup, AddTwoHeadersTest)
{
    struct ContactHeader *to = CreateToHeader();
    HeadersAddHeader(headers, (struct Header *)to);

    struct ContactHeader *from = CreateFromHeader();
    HeadersAddHeader(headers, (struct Header *)from);

    CHECK_EQUAL(2, HeadersLength(headers));
}

TEST(HeadersTestGroup, AddThreeHeadersTest)
{
    struct ContactHeader *to = CreateToHeader();
    HeadersAddHeader(headers, (struct Header *)to);

    struct ContactHeader *from = CreateFromHeader();
    HeadersAddHeader(headers, (struct Header *)from);

    struct ContactHeader *contact = CreateContactHeader();
    HeadersAddHeader(headers, (struct Header *)contact);

    CHECK_EQUAL(3, HeadersLength(headers));
}
