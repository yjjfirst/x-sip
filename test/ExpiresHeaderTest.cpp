#include "CppUTest/TestHarness.h"

extern "C" {
#include "ExpiresHeader.h"
}

TEST_GROUP(ExpiresHeaderTestGroup)
{
};

TEST(ExpiresHeaderTestGroup, ExpiresHeaderParseTest)
{
    struct ExpiresHeader *e = CreateExpiresHeader();
    char string[] = "Expires : 7200";

    Parse(string, e, GetExpiresPattern());
    STRCMP_EQUAL("Expires", ExpiresHeaderGetName(e));
    CHECK_EQUAL(7200, ExpiresHeaderGetExpires(e));
    DestoryExpiresHeader(e);
}

TEST(ExpiresHeaderTestGroup, ExpiresHeader2String)
{
    struct ExpiresHeader *e = CreateExpiresHeader();
    char string[] = "Expires:7200";
    char result[128] = {0};

    Parse(string, e, GetExpiresPattern());
    ExpiresHeader2String(result, e);

    DestoryExpiresHeader(e);
}