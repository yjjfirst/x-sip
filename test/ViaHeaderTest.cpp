#include "CppUTest/TestHarness.h"

extern "C" {
#include <string.h>
#include "Parser.h"
#include "ViaHeader.h"
}

TEST_GROUP(ViaHeaderTestGroup)
{
};

TEST(ViaHeaderTestGroup, ViaHeaderParseTest)
{
    struct ViaHeader *via = CreateViaHeader();
    char header[] = "Via   :  SIP/2.0/UDP erlang.bell-telephone.com:5060;branch=z9hG4bK87";

    Parse(header, via, GetViaPattern());
    STRCMP_EQUAL ("Via", ViaHeaderGetName(via));
    STRCMP_EQUAL ("SIP/2.0/UDP", ViaHeaderGetTransport(via));
    STRCMP_EQUAL ("erlang.bell-telephone.com:5060", ViaHeaderGetUri(via));
    STRCMP_EQUAL ("branch=z9hG4bK87", ViaHeaderGetParameters(via));

    DestoryViaHeader((struct Header *)via);
} 

TEST(ViaHeaderTestGroup, ViaHeaderWithNoParametersParseTest)
{
    struct ViaHeader *via = CreateViaHeader();
    char header[] = "Via   :  SIP/2.0/UDP erlang.bell-telephone.com:5060";

    Parse(header, via, GetViaPattern());
    STRCMP_EQUAL ("Via", ViaHeaderGetName(via));
    STRCMP_EQUAL ("SIP/2.0/UDP", ViaHeaderGetTransport(via));
    STRCMP_EQUAL ("erlang.bell-telephone.com:5060", ViaHeaderGetUri(via));

    DestoryViaHeader((struct Header *)via);
} 
