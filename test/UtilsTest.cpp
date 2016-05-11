#include "CppUTest/TestHarness.h"

extern "C" {
#include "StringExt.h"
}

TEST_GROUP(UtilsTestGroup)
{
};

TEST(UtilsTestGroup, StrcmpExtEqualTest)
{
    CHECK_EQUAL(0, StrcmpExt("", ""));
}

TEST(UtilsTestGroup, StrcmpExtUnequalTest)
{
    CHECK_EQUAL(-1, StrcmpExt("a", "b"));
}

TEST(UtilsTestGroup, StrcmpExtBothNummTest)
{
    CHECK_EQUAL(0, StrcmpExt(NULL, NULL));
}

TEST(UtilsTestGroup, StrcmpExtFirstNullTest)
{
    CHECK_EQUAL(-1, StrcmpExt(NULL, ""));
}

TEST(UtilsTestGroup, StrcmpExtSecondNullTest)
{
    CHECK_EQUAL(1, StrcmpExt("", NULL));
}
