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
    STRCMP_EQUAL("843E@TTT.COM", CallIdHeaderGetId(id));

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

TEST(CallIdHeaderTestGroup, CallIdHeaderDupTest)
{
    struct CallIdHeader *src = CreateEmptyCallIdHeader();
    char callidString[] = "Call-ID:843E@TTT.COM";

    Parse(callidString, src, GetCallIdPattern());
    struct CallIdHeader *dest = CallIdHeaderDup(src);
    
    CHECK_TRUE(CallIdHeaderMatched(src, dest));

    DestoryCallIdHeader((struct Header *)dest);
    DestoryCallIdHeader((struct Header *)src);
}

TEST(CallIdHeaderTestGroup, CallIdHeaderMatchedTest)
{
    struct CallIdHeader *id1 = CreateEmptyCallIdHeader();
    struct CallIdHeader *id2 = CreateEmptyCallIdHeader();

    char callidString[] = "Call-ID:843E@TTT.COM";

    Parse(callidString, id1, GetCallIdPattern());    
    Parse(callidString, id2, GetCallIdPattern());

    CHECK_TRUE(CallIdHeaderMatched(id1, id2));
    
    DestoryCallIdHeader((struct Header *)id1);
    DestoryCallIdHeader((struct Header *)id2);
}

TEST(CallIdHeaderTestGroup, CallIdHeaderUnmatchedTest)
{
    struct CallIdHeader *id1 = CreateEmptyCallIdHeader();
    struct CallIdHeader *id2 = CreateEmptyCallIdHeader();

    char callidString1[] = "Call-ID:843E@TTT.COM";
    char callidString2[] = "Call-ID:845E@TTT.COM";

    Parse(callidString1, id1, GetCallIdPattern());    
    Parse(callidString2, id2, GetCallIdPattern());

    CHECK_FALSE(CallIdHeaderMatched(id1, id2));
    
    DestoryCallIdHeader((struct Header *)id1);
    DestoryCallIdHeader((struct Header *)id2);
}
