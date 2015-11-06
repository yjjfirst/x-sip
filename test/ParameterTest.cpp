#include "CppUTest/TestHarness.h"

extern "C" {
#include "Parameter.h"
}

TEST_GROUP(ParameterTestGroup)
{
    struct Parameters *params;
    void setup(){
        params = CreateParameters();
    }

    void teardown() {
        DestoryParameters(params);
    }
};

TEST(ParameterTestGroup, OneParameterTest)
{
    char p[] = "branch=123456789";

    ParseParametersExt(p, params);
    
    STRCMP_EQUAL("123456789",GetParameter(params, (char *)"branch"));
    POINTERS_EQUAL(NULL, GetParameter(params, (char *)"subject"));
}

TEST(ParameterTestGroup, AnotherOneParameterTest)
{
    char p[] = "subject=test";

    ParseParametersExt(p, params);
    
    STRCMP_EQUAL("test",GetParameter(params, (char *)"subject"));
}

TEST(ParameterTestGroup, TwoParametersParseTest)
{
    char p[]="subject=test;branch=123456789";
    
    ParseParametersExt(p, params);
    STRCMP_EQUAL("test", GetParameter(params, (char*)"subject"));
    STRCMP_EQUAL("123456789", GetParameter(params, (char*)"branch"));
    POINTERS_EQUAL(NULL, GetParameter(params, (char*)"NoExistParameter"));
}

TEST(ParameterTestGroup, NoValueParameterParseTest)
{
    char p[]="subject=test;branch=123456789;rport;protocol=tcp;tport;transport=5060";
    
    ParseParametersExt(p, params);
    STRCMP_EQUAL("test", GetParameter(params, (char*)"subject"));
    STRCMP_EQUAL("123456789", GetParameter(params, (char*)"branch"));
    POINTERS_EQUAL(NULL, GetParameter(params, (char*)"NoExistParameter"));
    STRCMP_EQUAL("", GetParameter(params, (char *)"rport"));
    STRCMP_EQUAL("tcp", GetParameter(params, (char *)"protocol"));
    STRCMP_EQUAL("",GetParameter(params, (char *)"tport"));
    STRCMP_EQUAL("5060", GetParameter(params, (char *)"transport"));
}

TEST(ParameterTestGroup, ToStringTest)
{
    char p[]=";subject=test;branch=123456789;rport;protocol=tcp;tport;transport=5060";
    char result[128] = {0};

    ParseParametersExt(p, params);
    
    Parameters2StringExt(result, params, NULL);
    STRCMP_EQUAL(p, result);
}

TEST(ParameterTestGroup, AddParameterTest)
{
    AddParameter(params, (char *)"test1", (char *)"value1");
    STRCMP_EQUAL("value1", GetParameter(params, (char *)"test1"));

    AddParameter(params, (char *)"test2", (char *)"value2");
    STRCMP_EQUAL("value2", GetParameter(params, (char *)"test2"));

    AddParameter(params, (char *)"test3", (char *)"value3");
    STRCMP_EQUAL("value3", GetParameter(params, (char *)"test3"));

    AddParameter(params, (char *)"test3", (char *)"valuexxx");
    STRCMP_EQUAL("valuexxx", GetParameter(params, (char *)"test3"));

    AddParameter(params, (char *)"test2", (char *)"value2");
    STRCMP_EQUAL("value2", GetParameter(params, (char *)"test2"));
}
