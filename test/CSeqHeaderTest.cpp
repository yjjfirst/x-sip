#include "CppUTest/TestHarness.h"

extern "C" {
#include "CSeqHeader.h"
}

TEST_GROUP(CSeqTestGroup)
{
};

TEST(CSeqTestGroup, CSeqParseTest)
{
    struct CSeqHeader *c = CreateCSeqHeader(); 
    char header[] = "CSeq : 1826 REGISTER";

    Parse(header, c, GetCSeqParsePattern());
    STRCMP_EQUAL("CSeq", CSeqHeaderGetName(c));
    CHECK_EQUAL(1826, CSeqHeaderGetSeq(c));
    STRCMP_EQUAL("REGISTER", CSeqHeaderGetMethod(c));

    DestoryCSeqHeader(c);
}
