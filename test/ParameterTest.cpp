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
};

TEST(ParameterTestGroup, OneParameterTest)
{
    char p[] = "branch=123456789";

    ParseParametersExt(p, params);
    
    STRCMP_EQUAL("123456789",GetParameter(params, (char *)"branch"));
    POINTERS_EQUAL(NULL, GetParameter(params, (char *)"subject"));

    DestoryParameters(params);
}

TEST(ParameterTestGroup, AnotherOneParameterTest)
{
    char p[] = "subject=test";

    ParseParametersExt(p, params);
    
    STRCMP_EQUAL("test",GetParameter(params, (char *)"subject"));

    DestoryParameters(params);
}

TEST(ParameterTestGroup, TwoParametersParseTest)
{
    char p[]="subject=test;branch=123456789";
    
    ParseParametersExt(p, params);
    STRCMP_EQUAL("test", GetParameter(params, (char*)"subject"));
    STRCMP_EQUAL("123456789", GetParameter(params, (char*)"branch"));
    POINTERS_EQUAL(NULL, GetParameter(params, (char*)"NoExistParameter"));

    DestoryParameters(params);
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

    DestoryParameters(params);

}

TEST(ParameterTestGroup, ToStringTest)
{
    char p[]="subject=test;branch=123456789;rport;protocol=tcp;tport;transport=5060";
    char result[128] = {0};

    ParseParametersExt(p, params);
    
    Parameters2StringExt(result, params, NULL);
    STRCMP_EQUAL(p, result);

    DestoryParameters(params);
}
