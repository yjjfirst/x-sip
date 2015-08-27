#include "CppUTest/TestHarness.h"

extern "C" {
#include "MaxForwards.h"

}

TEST_GROUP(MaxForwardsTestGroup)
{
};

TEST(MaxForwardsTestGroup, MaxForwardsParseTest)
{
    struct MaxForwardsHeader *maxForwards = CreateMaxForwardsHeader();
    char maxForwardsString[] = "Max-Forwards : 69";

    Parse(maxForwardsString, maxForwards, GetMaxForwardPattern());
    
    STRCMP_EQUAL("Max-Forwards", MaxForwardsGetName(maxForwards));
    STRCMP_EQUAL("69", MaxForwardsGetMaxForwards(maxForwards));
    DestoryMaxForwardsHeader(maxForwards);
}
