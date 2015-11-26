#include "CppUTest/TestHarness.h"

extern "C" {
#include "Parser.h"
#include "CSeqHeader.h"
#include "SipMethod.h"
}

TEST_GROUP(CSeqTestGroup)
{
};

TEST(CSeqTestGroup, CSeqParseTest)
{
    struct CSeqHeader *c = CreateCSeqHeader(0, (char *)SIP_METHOD_NAME_NONE); 
    char header[] = "CSeq : 1826 REGISTER";

    Parse(header, c, GetCSeqHeaderPattern());
    STRCMP_EQUAL("CSeq", CSeqHeaderGetName(c));
    CHECK_EQUAL(1826, CSeqHeaderGetSeq(c));
    STRCMP_EQUAL("REGISTER", CSeqHeaderGetMethod(c));

    DestoryCSeqHeader((struct Header *)c);
}

TEST(CSeqTestGroup, CSeq2StringTest)
{
    struct CSeqHeader *c = CreateCSeqHeader(0, (char *)SIP_METHOD_NAME_NONE); 
    char header[] = "CSeq:1826 REGISTER";
    char result[128] = {0};

    Parse(header, c, GetCSeqHeaderPattern());
    CSeq2String(result,(struct Header *) c);
    STRCMP_EQUAL(header, result);

    DestoryCSeqHeader((struct Header *)c);
}

TEST(CSeqTestGroup, MethodMatchedTest)
{
    struct CSeqHeader *c1 = CreateCSeqHeader(0, (char *)SIP_METHOD_NAME_REGISTER);
    struct CSeqHeader *c2 = CreateCSeqHeader(0, (char *)SIP_METHOD_NAME_REGISTER);

    CHECK_TRUE(CSeqHeaderMethodMatched(c1, c2));
    
    DestoryCSeqHeader((struct Header *)c1);    
    DestoryCSeqHeader((struct Header *)c2);    
}

TEST(CSeqTestGroup, MethodUnMatchedTest)
{
    struct CSeqHeader *c1 = CreateCSeqHeader(0, (char *)SIP_METHOD_NAME_REGISTER);
    struct CSeqHeader *c2 = CreateCSeqHeader(0, (char *)SIP_METHOD_NAME_INVITE);

    CHECK_FALSE(CSeqHeaderMethodMatched(c1, c2));
    
    DestoryCSeqHeader((struct Header *)c1);    
    DestoryCSeqHeader((struct Header *)c2);    
}

TEST(CSeqTestGroup, MethodMatchedByString)
{
    struct CSeqHeader *c1 = CreateCSeqHeader(0, (char *)SIP_METHOD_NAME_REGISTER);

    CHECK_TRUE(CSeqMethodMatchedByString(c1, (char *)SIP_METHOD_NAME_REGISTER));

    CSeqHeaderSetMethod(c1, (char *)SIP_METHOD_NAME_INVITE);
    CHECK_FALSE(CSeqMethodMatchedByString(c1, (char *)SIP_METHOD_NAME_REGISTER));

    DestoryCSeqHeader((struct Header *)c1);    

}
