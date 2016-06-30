#include "CppUTest/TestHarness.h"

extern "C" {
#include "Parser.h"
#include "CSeqHeader.h"
#include "SipMethod.h"
}


TEST_GROUP(CSeqTestGroup)
{
    static unsigned int FakeCSeqGenerateSeq()
    {
        return 100;
    }

    void setup()
    {
        UT_PTR_SET(CSeqGenerateSeq, FakeCSeqGenerateSeq);
    }
};

TEST(CSeqTestGroup, CSeqParseTest)
{
    struct CSeqHeader *c = CreateCSeqHeader(0, (char *)SIP_METHOD_NAME_NONE); 
    char header[] = "CSeq : 1826 REGISTER";

    Parse(header, c, GetCSeqHeaderPattern());
    STRCMP_EQUAL("CSeq", CSeqHeaderGetName(c));
    CHECK_EQUAL(1826, CSeqHeaderGetSeq(c));
    STRCMP_EQUAL("REGISTER", CSeqHeaderGetMethod(c));

    DestroyCSeqHeader((struct Header *)c);
}

TEST(CSeqTestGroup, CSeq2StringTest)
{
    struct CSeqHeader *c = CreateCSeqHeader(0, (char *)SIP_METHOD_NAME_NONE); 
    char header[] = "CSeq:1826 REGISTER";
    char result[128] = {0};

    Parse(header, c, GetCSeqHeaderPattern());
    CSeq2String(result,(struct Header *) c);
    STRCMP_EQUAL(header, result);

    DestroyCSeqHeader((struct Header *)c);
}

TEST(CSeqTestGroup, MethodMatchedTest)
{
    struct CSeqHeader *c1 = CreateCSeqHeader(0, (char *)SIP_METHOD_NAME_REGISTER);
    struct CSeqHeader *c2 = CreateCSeqHeader(0, (char *)SIP_METHOD_NAME_REGISTER);

    CHECK_TRUE(CSeqHeaderMethodMatched(c1, c2));
    
    DestroyCSeqHeader((struct Header *)c1);    
    DestroyCSeqHeader((struct Header *)c2);    
}

TEST(CSeqTestGroup, MethodUnMatchedTest)
{
    struct CSeqHeader *c1 = CreateCSeqHeader(0, (char *)SIP_METHOD_NAME_REGISTER);
    struct CSeqHeader *c2 = CreateCSeqHeader(0, (char *)SIP_METHOD_NAME_INVITE);

    CHECK_FALSE(CSeqHeaderMethodMatched(c1, c2));
    
    DestroyCSeqHeader((struct Header *)c1);    
    DestroyCSeqHeader((struct Header *)c2);    
}

TEST(CSeqTestGroup, MethodMatchedByString)
{
    struct CSeqHeader *c1 = CreateCSeqHeader(0, (char *)SIP_METHOD_NAME_REGISTER);

    CHECK_TRUE(CSeqMethodMatchedByName(c1, (char *)SIP_METHOD_NAME_REGISTER));

    CSeqHeaderSetMethod(c1, (char *)SIP_METHOD_NAME_INVITE);
    CHECK_FALSE(CSeqMethodMatchedByName(c1, (char *)SIP_METHOD_NAME_REGISTER));

    DestroyCSeqHeader((struct Header *)c1);    

}

TEST(CSeqTestGroup, CSeqHeadersMatchedTest)
{
    struct CSeqHeader *c1 = CreateCSeqHeader(101, (char *)SIP_METHOD_NAME_REGISTER);
    struct CSeqHeader *c2 = CreateCSeqHeader(101, (char *)SIP_METHOD_NAME_REGISTER);

    CHECK_TRUE(CSeqHeadersMatched(c1, c2));
    
    DestroyCSeqHeader((struct Header *)c1);    
    DestroyCSeqHeader((struct Header *)c2);    

}

TEST(CSeqTestGroup, CSeqHeadersUnmatchedTest)
{
    struct CSeqHeader *c1 = CreateCSeqHeader(101, (char *)SIP_METHOD_NAME_REGISTER);
    struct CSeqHeader *c2 = CreateCSeqHeader(102, (char *)SIP_METHOD_NAME_REGISTER);

    CHECK_FALSE(CSeqHeadersMatched(c1, c2));
    
    DestroyCSeqHeader((struct Header *)c1);    
    DestroyCSeqHeader((struct Header *)c2);    
}

TEST(CSeqTestGroup, CSeqHeaderDupTest)
{
    struct CSeqHeader *src = CreateCSeqHeader(101, (char *)SIP_METHOD_NAME_REGISTER);
    struct CSeqHeader *dest = CSeqHeaderDup(src);

    CHECK_TRUE(CSeqHeadersMatched(src, dest));

    DestroyCSeqHeader((struct Header *)src);
    DestroyCSeqHeader((struct Header *)dest);
}
