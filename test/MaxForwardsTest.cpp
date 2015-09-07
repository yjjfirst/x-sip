#include "CppUTest/TestHarness.h"

extern "C" {
#include "MaxForwards.h"
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

    DestoryMaxForwardsHeader(maxForwards);
}
