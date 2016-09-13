#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include <string.h>
#include <stdio.h>
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
    VIA_HEADER *via = CreateEmptyViaHeader();
    char header[] = "Via   :  SIP/2.0/UDP erlang.bell-telephone.com:5060;branch=z9hG4bK87";

    Parse(header, via, GetViaPattern());
    STRCMP_EQUAL ("Via", ViaHeaderGetName(via));
    STRCMP_EQUAL ("SIP/2.0/UDP", ViaHeaderGetTransport(via));

    URI *u = ViaHeaderGetUri(via);
    STRCMP_EQUAL("erlang.bell-telephone.com", UriGetHost(u));
    CHECK_EQUAL(5060, UriGetPort(u));

    STRCMP_EQUAL("z9hG4bK87",ViaHeaderGetParameter(via, (char *)VIA_BRANCH_PARAMETER_NAME));
    DestroyViaHeader((struct Header *)via);
} 

TEST(ViaHeaderTestGroup, ViaHeaderWithNoParametersParseTest)
{
    VIA_HEADER *via = CreateEmptyViaHeader();
    char header[] = "Via   :  SIP/2.0/UDP erlang.bell-telephone.com:5060";

    Parse(header, via, GetViaPattern());
    STRCMP_EQUAL ("Via", ViaHeaderGetName(via));
    STRCMP_EQUAL ("SIP/2.0/UDP", ViaHeaderGetTransport(via));

    URI *u = ViaHeaderGetUri(via);
    STRCMP_EQUAL("erlang.bell-telephone.com", UriGetHost(u));
    CHECK_EQUAL(5060, UriGetPort(u));

    DestroyViaHeader((struct Header *)via);
} 

TEST(ViaHeaderTestGroup, ViaHeader2StringTest)
{
    VIA_HEADER *via = CreateEmptyViaHeader();
    char header[] = "Via   :  SIP/2.0/UDP erlang.bell-telephone.com:5060";
    URI *uri;
    char result[128] = {0};

    Parse(header, via, GetViaPattern());
    STRCMP_EQUAL("Via", ViaHeaderGetName(via));
    STRCMP_EQUAL("SIP/2.0/UDP", ViaHeaderGetTransport(via));

    uri = ViaHeaderGetUri(via);
    STRCMP_EQUAL("erlang.bell-telephone.com",UriGetHost(uri));

    ViaHeader2String(result, (struct Header *)via);

    STRCMP_EQUAL("Via:SIP/2.0/UDP erlang.bell-telephone.com:5060", result);

    DestroyViaHeader((struct Header *)via);
}

TEST(ViaHeaderTestGroup, UnMatchedBranchTest)
{
    VIA_HEADER *via1 = CreateEmptyViaHeader();
    VIA_HEADER *via2 = CreateEmptyViaHeader();

    char header1[] = "Via   :  SIP/2.0/UDP erlang.bell-telephone.com:5060;branch=asdfcdz9hG4bK87";
    char header2[] = "Via   :  SIP/2.0/UDP erlang.bell-telephone.com:5060;branch=z9hG4bK87";

    Parse(header1, via1, GetViaPattern());
    Parse(header2, via2, GetViaPattern());

    CHECK_FALSE(ViaHeaderBranchMatched(via1, via2));

    DestroyViaHeader((struct Header *)via1);
    DestroyViaHeader((struct Header *)via2);

}

TEST(ViaHeaderTestGroup, MatchedBranchTest)
{
    VIA_HEADER *via1 = CreateEmptyViaHeader();
    VIA_HEADER *via2 = CreateEmptyViaHeader();

    char header1[] = "Via   :  SIP/2.0/UDP erlang.bell-telephone.com:5060;branch=z9hG4bK87";
    char header2[] = "Via   :  SIP/2.0/UDP erlang.bell-telephone.com:5060;branch=z9hG4bK87";

    Parse(header1, via1, GetViaPattern());
    Parse(header2, via2, GetViaPattern());

    CHECK_TRUE(ViaHeaderBranchMatched(via1, via2));

    DestroyViaHeader((struct Header *)via1);
    DestroyViaHeader((struct Header *)via2);
}

TEST(ViaHeaderTestGroup, MatchBranchByStringTest)
{
    VIA_HEADER *via1 = CreateEmptyViaHeader();
    char header1[] = "Via   :  SIP/2.0/UDP erlang.bell-telephone.com:5060;branch=z9hG4bK87";
    
    Parse(header1, via1, GetViaPattern());
    CHECK_TRUE(ViaHeaderBranchMatchedByString(via1, (char *)"z9hG4bK87"));

    struct Parameters *ps = ViaHeaderGetParameters(via1);
    AddParameter(ps, (char *)VIA_BRANCH_PARAMETER_NAME, (char *)"12345678");
    CHECK_FALSE(ViaHeaderBranchMatchedByString(via1, (char *)"z9hG4bK87"));

    DestroyViaHeader((struct Header *)via1);
}

TEST(ViaHeaderTestGroup, ViaSendbyMatchedTest)
{
    char string[] = "Via: SIP/2.0/UDP 192.168.10.1:5060;branch=z9hG4bK56fb2ea6;rport;send-by=192.168.10.101:5060";
    VIA_HEADER *via1 = CreateEmptyViaHeader();
    VIA_HEADER *via2 = CreateEmptyViaHeader();

    Parse(string, via1, GetViaPattern());
    Parse(string, via2, GetViaPattern());
    CHECK_TRUE(ViaHeaderSendbyMatched(via1, via2));

    DestroyViaHeader((struct Header *)via1);
    DestroyViaHeader((struct Header *)via2);

}

TEST(ViaHeaderTestGroup, ViaSendbyUnmatchedTest)
{
    char string[] = "Via: SIP/2.0/UDP 192.168.10.1:5060;branch=z9hG4bK56fb2ea6;rport;send-by=192.168.10.101:5060";
    VIA_HEADER *via1 = CreateEmptyViaHeader();
    VIA_HEADER *via2 = CreateEmptyViaHeader();

    Parse(string, via1, GetViaPattern());
    Parse(string, via2, GetViaPattern());
    ViaHeaderSetParameter(via2, VIA_SENDBY_PARAMETER_NAME, (char *)"192.168.10.111:5061");

    CHECK_FALSE(ViaHeaderSendbyMatched(via1, via2));

    DestroyViaHeader((struct Header *)via1);
    DestroyViaHeader((struct Header *)via2);

}

TEST(ViaHeaderTestGroup, ViaMatchedTest)
{
    VIA_HEADER *via1 = CreateEmptyViaHeader();
    VIA_HEADER *via2 = CreateEmptyViaHeader();
    char header1[] = "Via:SIP/2.0/UDP erlang.bell-telephone.com:5060;branch=z9hG4bK87";
    Parse(header1, via1, GetViaPattern());
    Parse(header1, via2, GetViaPattern());

    CHECK_TRUE(ViaHeaderMatched(via1, via2));

    DestroyViaHeader((struct Header *)via1);
    DestroyViaHeader((struct Header *)via2);
}

TEST(ViaHeaderTestGroup, ViaUriUnmatchedTest)
{
    VIA_HEADER *via1 = CreateEmptyViaHeader();
    VIA_HEADER *via2 = CreateEmptyViaHeader();
    char header1[] = "Via:SIP/2.0/UDP erlang.bell-telephone.com:5060;branch=z9hG4bK87";
    char header2[] = "Via:SIP/2.0/UDP erlang.bell-telephone.com:5061;branch=z9hG4bK87";
    Parse(header1, via1, GetViaPattern());
    Parse(header2, via2, GetViaPattern());

    CHECK_FALSE(ViaHeaderMatched(via1, via2));

    DestroyViaHeader((struct Header *)via1);
    DestroyViaHeader((struct Header *)via2);
}

TEST(ViaHeaderTestGroup, ViaTransportUnmatchedTest)
{
    VIA_HEADER *via1 = CreateEmptyViaHeader();
    VIA_HEADER *via2 = CreateEmptyViaHeader();
    char header1[] = "Via:SIP/2.0/UDP erlang.bell-telephone.com:5060;branch=z9hG4bK87";
    char header2[] = "Via:SIP/2.0/TCP erlang.bell-telephone.com:5060;branch=z9hG4bK87";
    Parse(header1, via1, GetViaPattern());
    Parse(header2, via2, GetViaPattern());

    CHECK_FALSE(ViaHeaderMatched(via1, via2));

    DestroyViaHeader((struct Header *)via1);
    DestroyViaHeader((struct Header *)via2);
}

TEST(ViaHeaderTestGroup, ViaParameterUnmatchedTest)
{
    VIA_HEADER *via1 = CreateEmptyViaHeader();
    VIA_HEADER *via2 = CreateEmptyViaHeader();
    char header1[] = "Via:SIP/2.0/UDP erlang.bell-telephone.com:5060;branch=z9hG4bK87";
    char header2[] = "Via:SIP/2.0/UDP erlang.bell-telephone.com:5060;branch=111111111";
    Parse(header1, via1, GetViaPattern());
    Parse(header2, via2, GetViaPattern());

    CHECK_FALSE(ViaHeaderMatched(via1, via2));

    DestroyViaHeader((struct Header *)via1);
    DestroyViaHeader((struct Header *)via2);
}

TEST(ViaHeaderTestGroup, ViaHeaderSetParameterTest)
{
    VIA_HEADER *via = CreateEmptyViaHeader();
    char string[] = "Via:SIP/2.0/UDP erlang.bell-telephone.com:5060;branch=z9hG4bK87";
    Parse(string, via, GetViaPattern());

    STRCMP_EQUAL("z9hG4bK87", ViaHeaderGetParameter(via, VIA_BRANCH_PARAMETER_NAME));
    ViaHeaderSetParameter(via, VIA_BRANCH_PARAMETER_NAME, (char *)"1234567890");
    STRCMP_EQUAL("1234567890", ViaHeaderGetParameter(via, VIA_BRANCH_PARAMETER_NAME));

    DestroyViaHeader((struct Header *)via);
}

TEST(ViaHeaderTestGroup, ViaHeaderDupTest)
{
    VIA_HEADER *src = CreateEmptyViaHeader();
    char string[] = "Via:SIP/2.0/UDP erlang.bell-telephone.com:5060;branch=z9hG4bK87";
    Parse(string, src, GetViaPattern());
  
    VIA_HEADER *dest = ViaHeaderDup(src);
    CHECK_TRUE(ViaHeaderMatched(src, dest));

    DestroyViaHeader((struct Header *)src);
    DestroyViaHeader((struct Header *)dest);
}

TEST(ViaHeaderTestGroup, GenerateBranchTest)
{
    char branch[32];

    GenerateBranch(branch);

    STRNCMP_EQUAL(VIA_BRANCH_COOKIE, branch, VIA_BRANCH_COOKIE_LENGTH);
    
    for (int i = VIA_BRANCH_COOKIE_LENGTH; i < VIA_BRANCH_LENGTH; i++) {        
        CHECK_TRUE( isdigit(branch[i]));
    }

    CHECK_EQUAL(VIA_BRANCH_LENGTH, strlen(branch));
}
