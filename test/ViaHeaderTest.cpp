#include "CppUTest/TestHarness.h"

extern "C" {
#include <string.h>
#include "Parser.h"
#include "ViaHeader.h"
#include "URI.h"
#include "Parameter.h"
}

TEST_GROUP(ViaHeaderTestGroup)
{
};

TEST(ViaHeaderTestGroup, ViaHeaderParseTest)
{
    struct ViaHeader *via = CreateEmptyViaHeader();
    char header[] = "Via   :  SIP/2.0/UDP erlang.bell-telephone.com:5060;branch=z9hG4bK87";

    Parse(header, via, GetViaPattern());
    STRCMP_EQUAL ("Via", ViaHeaderGetName(via));
    STRCMP_EQUAL ("SIP/2.0/UDP", ViaHeaderGetTransport(via));

    struct URI *u = ViaHeaderGetUri(via);
    STRCMP_EQUAL("erlang.bell-telephone.com", UriGetHost(u));
    CHECK_EQUAL(5060, UriGetPort(u));

    STRCMP_EQUAL("z9hG4bK87",ViaHeaderGetParameter(via, (char *)"branch"));
    DestoryViaHeader((struct Header *)via);
} 

TEST(ViaHeaderTestGroup, ViaHeaderWithNoParametersParseTest)
{
    struct ViaHeader *via = CreateEmptyViaHeader();
    char header[] = "Via   :  SIP/2.0/UDP erlang.bell-telephone.com:5060";

    Parse(header, via, GetViaPattern());
    STRCMP_EQUAL ("Via", ViaHeaderGetName(via));
    STRCMP_EQUAL ("SIP/2.0/UDP", ViaHeaderGetTransport(via));

    struct URI *u = ViaHeaderGetUri(via);
    STRCMP_EQUAL("erlang.bell-telephone.com", UriGetHost(u));
    CHECK_EQUAL(5060, UriGetPort(u));

    DestoryViaHeader((struct Header *)via);
} 

TEST(ViaHeaderTestGroup, ViaHeader2StringTest)
{
    struct ViaHeader *via = CreateEmptyViaHeader();
    char header[] = "Via   :  SIP/2.0/UDP erlang.bell-telephone.com:5060";
    struct URI *uri;
    char result[128] = {0};

    Parse(header, via, GetViaPattern());
    STRCMP_EQUAL("Via", ViaHeaderGetName(via));
    STRCMP_EQUAL("SIP/2.0/UDP", ViaHeaderGetTransport(via));

    uri = ViaHeaderGetUri(via);
    STRCMP_EQUAL("erlang.bell-telephone.com",UriGetHost(uri));

    ViaHeader2String(result, (struct Header *)via);

    STRCMP_EQUAL("Via:SIP/2.0/UDP erlang.bell-telephone.com:5060", result);

    DestoryViaHeader((struct Header *)via);
}

TEST(ViaHeaderTestGroup, UnMatchedBranchTest)
{
    struct ViaHeader *via1 = CreateEmptyViaHeader();
    struct ViaHeader *via2 = CreateEmptyViaHeader();

    char header1[] = "Via   :  SIP/2.0/UDP erlang.bell-telephone.com:5060;branch=asdfcdz9hG4bK87";
    char header2[] = "Via   :  SIP/2.0/UDP erlang.bell-telephone.com:5060;branch=z9hG4bK87";

    Parse(header1, via1, GetViaPattern());
    Parse(header2, via2, GetViaPattern());

    CHECK_FALSE(ViaBranchMatched(via1, via2));

    DestoryViaHeader((struct Header *)via1);
    DestoryViaHeader((struct Header *)via2);

}

TEST(ViaHeaderTestGroup, MatchedBranchTest)
{
    struct ViaHeader *via1 = CreateEmptyViaHeader();
    struct ViaHeader *via2 = CreateEmptyViaHeader();

    char header1[] = "Via   :  SIP/2.0/UDP erlang.bell-telephone.com:5060;branch=z9hG4bK87";
    char header2[] = "Via   :  SIP/2.0/UDP erlang.bell-telephone.com:5060;branch=z9hG4bK87";

    Parse(header1, via1, GetViaPattern());
    Parse(header2, via2, GetViaPattern());

    CHECK_TRUE(ViaBranchMatched(via1, via2));

    DestoryViaHeader((struct Header *)via1);
    DestoryViaHeader((struct Header *)via2);
}
