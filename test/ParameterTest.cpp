#include "CppUTest/TestHarness.h"

extern "C" {
#include "Parameter.h"
}

TEST_GROUP(ParameterTestGroup)
{
};

TEST(ParameterTestGroup, OneParameterTest)
{
    char p[] = "branch=123456789";

    struct Parameters *param = ParseParameters(p);
    
    STRCMP_EQUAL("123456789",GetParameter(param, (char *)"branch"));
    POINTERS_EQUAL(NULL, GetParameter(param, (char *)"subject"));

    DestoryParameters(param);
}

TEST(ParameterTestGroup, AnotherOneParameterTest)
{
    char p[] = "subject=test";

    struct Parameters *param = ParseParameters(p);
    
    STRCMP_EQUAL("test",GetParameter(param, (char *)"subject"));

    DestoryParameters(param);
}
