#include "CppUTest/TestHarness.h"

extern "C" {
#include "StatusLine.h"
}

TEST_GROUP(StatusLineTestGroup)
{
};

TEST(StatusLineTestGroup, StatusLineParseTest)
{
    struct StatusLine *s = CreateStatusLine(0, (char *)"");
    char string[] = "SIP/2.0 180 Ringing";

    ParseStatusLine(string, s);
    STRCMP_EQUAL("SIP/2.0", GetSIPVersion(s));
    CHECK_EQUAL(180, GetStatusCode(s));
    STRCMP_EQUAL("Ringing", GetReasonPhrase(s));

    DestroyStatusLine(s);
}

TEST(StatusLineTestGroup, StatusLine2StringTest)
{
    struct StatusLine *s = CreateStatusLine(0, (char *)"");
    char string[] = "SIP/2.0 180 Ringing";
    char result[64] = {0};

    ParseStatusLine(string, s);
    StatusLine2String(result, s);
    STRCMP_EQUAL(string, result);    

    DestroyStatusLine(s);

}

TEST(StatusLineTestGroup, Create100StatusLineTest)
{
    struct StatusLine *s = CreateStatusLine(100, "Trying");
    CHECK_EQUAL(100, GetStatusCode(s));
    STRCMP_EQUAL("Trying", GetReasonPhrase(s));

    DestroyStatusLine(s);
}

TEST(StatusLineTestGroup, Create180StatusLineTest)
{
    struct StatusLine *s = CreateStatusLine(180, "Ringing");
    CHECK_EQUAL(180, GetStatusCode(s));
    STRCMP_EQUAL("Ringing", GetReasonPhrase(s));

    DestroyStatusLine(s);
}

TEST(StatusLineTestGroup, StatusLineCreateTest)
{
    struct StatusLine *s = CreateStatusLine(180, NULL);
    CHECK_EQUAL(180, GetStatusCode(s));
    STRCMP_EQUAL("", GetReasonPhrase(s));

    DestroyStatusLine(s);
}

