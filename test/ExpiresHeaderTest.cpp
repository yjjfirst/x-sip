#include "CppUTest/TestHarness.h"

extern "C" {
#include "Parser.h"
#include "ExpiresHeader.h"
}

TEST_GROUP(ExpiresHeaderTestGroup)
{
};

TEST(ExpiresHeaderTestGroup, ExpiresHeaderParseTest)
{
    struct ExpiresHeader *e = CreateExpiresHeader(7777);
    char string[] = "Expires : 7200";

    CHECK_EQUAL(7777, ExpiresHeaderGetExpires(e));

    Parse(string, e, GetExpiresPattern());
    STRCMP_EQUAL("Expires", ExpiresHeaderGetName(e));
    CHECK_EQUAL(7200, ExpiresHeaderGetExpires(e));
    DestoryExpiresHeader((struct Header *)e);
}

TEST(ExpiresHeaderTestGroup, ExpiresHeader2String)
{
    struct ExpiresHeader *e = CreateExpiresHeader(0);
    char string[] = "Expires:7200";
    char result[128] = {0};

    Parse(string, e, GetExpiresPattern());
    ExpiresHeader2String(result, (struct Header *)e);

    DestoryExpiresHeader((struct Header *)e);
}
