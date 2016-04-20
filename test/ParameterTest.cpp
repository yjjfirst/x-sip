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

TEST(ParameterTestGroup, ParameterMatchedTest)
{
    struct Parameter *p1 = CreateParameter((char *)"1", (char *)"2");
    struct Parameter *p2 = CreateParameter((char *)"1", (char *)"2");

    CHECK_TRUE(ParameterMatched(p1, p2));
    
    DestoryParameter(p1);
    DestoryParameter(p2);
}

TEST(ParameterTestGroup, ParameterUnmatchedTest)
{
    struct Parameter *p1 = CreateParameter((char *)"1", (char *)"2");
    struct Parameter *p2 = CreateParameter((char *)"4", (char *)"3");

    CHECK_FALSE(ParameterMatched(p1, p2));
    
    DestoryParameter(p1);
    DestoryParameter(p2);
}

TEST(ParameterTestGroup, ParametersMatchedTest)
{
    struct Parameters *ps = CreateParameters();

    AddParameter(params, (char *)"test1", (char *)"value1");
    AddParameter(params, (char *)"test2", (char *)"value2");
    AddParameter(params, (char *)"test3", (char *)"value3");

    AddParameter(ps, (char *)"test1", (char *)"value1");
    AddParameter(ps, (char *)"test2", (char *)"value2");
    AddParameter(ps, (char *)"test3", (char *)"value3");

    CHECK_TRUE(ParametersMatched(params, ps));

    DestoryParameters(ps);
}

TEST(ParameterTestGroup, ParametersUnmatchedTest)
{
    struct Parameters *ps = CreateParameters();

    AddParameter(params, (char *)"test1", (char *)"value1");
    AddParameter(params, (char *)"test2", (char *)"value2");
    AddParameter(params, (char *)"test3", (char *)"value3");

    AddParameter(ps, (char *)"test2", (char *)"value1");
    AddParameter(ps, (char *)"test2", (char *)"value2");
    AddParameter(ps, (char *)"test3", (char *)"value3");

    CHECK_FALSE(ParametersMatched(params, ps));

    DestoryParameters(ps);
}

TEST(ParameterTestGroup, ParametersLengthUnmatchedTest)
{
    struct Parameters *ps = CreateParameters();

    AddParameter(params, (char *)"test1", (char *)"value1");
    AddParameter(params, (char *)"test2", (char *)"value2");
    AddParameter(params, (char *)"test3", (char *)"value3");

    AddParameter(ps, (char *)"test1", (char *)"value1");
    AddParameter(ps, (char *)"test2", (char *)"value2");

    CHECK_FALSE(ParametersMatched(params, ps));

    DestoryParameters(ps);
}

TEST(ParameterTestGroup, NullElementParametersDupTest)
{
    struct Parameters *ps = CreateParameters();
    struct Parameters *dest = ParametersDup(ps);
    CHECK_TRUE(ParametersMatched(ps, dest));

    DestoryParameters(dest);
    DestoryParameters(ps);

}

TEST(ParameterTestGroup, OneElementParametersDupTest)
{
    struct Parameters *ps = CreateParameters();

    AddParameter(ps, (char *)"test1", (char *)"value1");

    struct Parameters *dest = ParametersDup(ps);
    CHECK_TRUE(ParametersMatched(ps, dest));

    DestoryParameters(dest);
    DestoryParameters(ps);


}

TEST(ParameterTestGroup, ThreeElementParametersDupTest)
{
    struct Parameters *ps = CreateParameters();

    AddParameter(ps, (char *)"test1", (char *)"value1");
    AddParameter(ps, (char *)"test2", (char *)"value2");
    AddParameter(ps, (char *)"test3", (char *)"value3");

    struct Parameters *dest = ParametersDup(ps);
    CHECK_TRUE(ParametersMatched(ps, dest));

    DestoryParameters(dest);
    DestoryParameters(ps);


}

TEST(ParameterTestGroup, TenElementParametersDupTest)
{
    struct Parameters *ps = CreateParameters();

    AddParameter(ps, (char *)"test1", (char *)"value1");
    AddParameter(ps, (char *)"test2", (char *)"value2");
    AddParameter(ps, (char *)"test3", (char *)"value3");
    AddParameter(ps, (char *)"test4", (char *)"value4");
    AddParameter(ps, (char *)"test5", (char *)"value6");
    AddParameter(ps, (char *)"test6", (char *)"value7");
    AddParameter(ps, (char *)"test7", (char *)"value8");
    AddParameter(ps, (char *)"test8", (char *)"value9");
    AddParameter(ps, (char *)"test9", (char *)"value10");
    AddParameter(ps, (char *)"test10", (char *)"value5");

    struct Parameters *dest = ParametersDup(ps);
    CHECK_TRUE(ParametersMatched(ps, dest));

    DestoryParameters(dest);
    DestoryParameters(ps);
}

TEST(ParameterTestGroup, ClearParametersTest)
{
    struct Parameters *ps = CreateParameters();

    AddParameter(ps, (char *)"test1", (char *)"value1");
    AddParameter(ps, (char *)"test2", (char *)"value2");
    AddParameter(ps, (char *)"test3", (char *)"value3");
    AddParameter(ps, (char *)"test4", (char *)"value4");
    AddParameter(ps, (char *)"test5", (char *)"value6");
    AddParameter(ps, (char *)"test6", (char *)"value7");
    AddParameter(ps, (char *)"test7", (char *)"value8");
    AddParameter(ps, (char *)"test8", (char *)"value9");
    AddParameter(ps, (char *)"test9", (char *)"value10");
    AddParameter(ps, (char *)"test10", (char *)"value5");

    ClearParameters(ps);
    CHECK_EQUAL(0, ParametersLength(ps));
    
    DestoryParameters(ps);
}
