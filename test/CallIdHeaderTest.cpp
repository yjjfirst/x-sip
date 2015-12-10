#include "CppUTest/TestHarness.h"

extern "C" {
#include "CallIdHeader.h"
#include "Parser.h"
}

TEST_GROUP(CallIdHeaderTestGroup)
{
};

TEST(CallIdHeaderTestGroup, CallIdHeaderParseTest)
{
    struct CallIdHeader *id = CreateEmptyCallIdHeader();
    char callidString[] = "Call-ID :  843E@TTT.COM";

    Parse(callidString, id, GetCallIdPattern());
    STRCMP_EQUAL("Call-ID", CallIdHeaderGetName(id));
    STRCMP_EQUAL("843E@TTT.COM", CallIdHeaderGetID(id));

    DestoryCallIdHeader((struct Header *)id);
}

TEST(CallIdHeaderTestGroup, CallIdHeader2StringTest)
{
    struct CallIdHeader *id = CreateEmptyCallIdHeader();
    char callidString[] = "Call-ID:843E@TTT.COM";
    char result[128] = {0};

    Parse(callidString, id, GetCallIdPattern());
    CallIdHeader2String(result, (struct Header *)id);
    STRCMP_EQUAL(callidString, result);

    DestoryCallIdHeader((struct Header *)id);
}
