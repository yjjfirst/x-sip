#include "CppUTest/TestHarness.h"

extern "C" {
#include "CallIDHeader.h"
#include "Parser.h"
}

TEST_GROUP(CallIDHeaderTestGroup)
{
};

TEST(CallIDHeaderTestGroup, CallIDHeaderParseTest)
{
    struct CallIDHeader *id = CreateCallIDHeader();
    char callidString[] = "Call-ID :  843E@TTT.COM";

    Parse(callidString, id, GetCallIDPattern());
    STRCMP_EQUAL("Call-ID", CallIDHeaderGetName(id));
    STRCMP_EQUAL("843E@TTT.COM", CallIDHeaderGetID(id));

    DestoryCallIDHeader((struct Header *)id);
}

TEST(CallIDHeaderTestGroup, CallIDHeader2StringTest)
{
    struct CallIDHeader *id = CreateCallIDHeader();
    char callidString[] = "Call-ID:843E@TTT.COM";
    char result[128] = {0};

    Parse(callidString, id, GetCallIDPattern());
    CallIDHeader2String(result, id);
    STRCMP_EQUAL(callidString, result);

    DestoryCallIDHeader((struct Header *)id);
}
