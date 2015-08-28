#include "CppUTest/TestHarness.h"

extern "C" {
#include "CallIDHeader.h"
}

TEST_GROUP(CALLIDHeaderTestGroup)
{
};

TEST(CALLIDHeaderTestGroup, CALLIDHeaderParseTest)
{
    struct CallIDHeader *id = CreateCallIDHeader();
    char callidString[] = "Call-ID :  843E@TTT.COM";

    Parse(callidString, id, GetCallIDPattern());
    STRCMP_EQUAL("Call-ID", CallIDHeaderGetName(id));
    STRCMP_EQUAL("843E@TTT.COM", CallIDHeaderGetID(id));
    DestoryCallIDHeader(id);
}
