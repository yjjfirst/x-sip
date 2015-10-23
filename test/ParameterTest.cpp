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

TEST(ParameterTestGroup, TwoParametersParseTest)
{
    char p[]="subject=test;branch=123456789";
    
    struct Parameters *param = ParseParameters(p);
    STRCMP_EQUAL("test", GetParameter(param, (char*)"subject"));
    STRCMP_EQUAL("123456789", GetParameter(param, (char*)"branch"));
    POINTERS_EQUAL(NULL, GetParameter(param, (char*)"NoExistParameter"));

    DestoryParameters(param);
}

TEST(ParameterTestGroup, NoValueParameterParseTest)
{
    char p[]="subject=test;branch=123456789;rport;protocol=tcp;tport;transport=5060";
    
    struct Parameters *param = ParseParameters(p);
    STRCMP_EQUAL("test", GetParameter(param, (char*)"subject"));
    STRCMP_EQUAL("123456789", GetParameter(param, (char*)"branch"));
    POINTERS_EQUAL(NULL, GetParameter(param, (char*)"NoExistParameter"));
    STRCMP_EQUAL("", GetParameter(param, (char *)"rport"));
    STRCMP_EQUAL("tcp", GetParameter(param, (char *)"protocol"));
    STRCMP_EQUAL("",GetParameter(param, (char *)"tport"));
    STRCMP_EQUAL("5060", GetParameter(param, (char *)"transport"));

    DestoryParameters(param);

}

TEST(ParameterTestGroup, ToStringTest)
{
    char p[]="subject=test;branch=123456789;rport;protocol=tcp;tport;transport=5060";
    char result[128] = {0};

    struct Parameters *param = ParseParameters(p);
    
    Parameters2String(param, result);
    STRCMP_EQUAL(p, result);

    DestoryParameters(param);
}
