#include "CppUTest/TestHarness.h"

extern "C" {
#include "Parser.h"
#include "CSeqHeader.h"
}

TEST_GROUP(CSeqTestGroup)
{
};

TEST(CSeqTestGroup, CSeqParseTest)
{
    struct CSeqHeader *c = CreateCSeqHeader(); 
    char header[] = "CSeq : 1826 REGISTER";

    Parse(header, c, GetCSeqHeaderPattern());
    STRCMP_EQUAL("CSeq", CSeqHeaderGetName(c));
    CHECK_EQUAL(1826, CSeqHeaderGetSeq(c));
    STRCMP_EQUAL("REGISTER", CSeqHeaderGetMethod(c));

    DestoryCSeqHeader((struct Header *)c);
}

TEST(CSeqTestGroup, CSeq2StringTest)
{
    struct CSeqHeader *c = CreateCSeqHeader(); 
    char header[] = "CSeq:1826 REGISTER";
    char result[128] = {0};

    Parse(header, c, GetCSeqHeaderPattern());
    CSeq2String(result, c);
    STRCMP_EQUAL(header, result);

    DestoryCSeqHeader((struct Header *)c);
}
