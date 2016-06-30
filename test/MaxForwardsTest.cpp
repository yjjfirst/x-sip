#include "CppUTest/TestHarness.h"

extern "C" {
#include "MaxForwardsHeader.h"
#include "Parser.h"
}

TEST_GROUP(MaxForwardsTestGroup)
{
};

TEST(MaxForwardsTestGroup, MaxForwardsParseTest)
{
    struct MaxForwardsHeader *maxForwards = CreateMaxForwardsHeader();
    char maxForwardsString[] = "Max-Forwards : 69";

    Parse(maxForwardsString, maxForwards, GetMaxForwardsPattern());
    
    STRCMP_EQUAL("Max-Forwards", MaxForwardsGetName(maxForwards));
    CHECK_EQUAL(69, MaxForwardsGetMaxForwards(maxForwards));

    DestroyMaxForwardsHeader((struct Header *)maxForwards);
}

TEST(MaxForwardsTestGroup, MaxForwards2StringTest)
{
    struct MaxForwardsHeader *maxForwards = CreateMaxForwardsHeader();
    char maxForwardsString[] = "Max-Forwards:69";
    char result[64] = {0};

    Parse(maxForwardsString, maxForwards, GetMaxForwardsPattern());
    MaxForwards2String(result,(struct Header *) maxForwards);
    STRCMP_EQUAL(maxForwardsString, result);

    DestroyMaxForwardsHeader((struct Header *)maxForwards);
}


