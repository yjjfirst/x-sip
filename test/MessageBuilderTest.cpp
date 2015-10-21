#include "CppUTest/TestHarness.h"

extern "C" {
#include "MessageBuilder.h"
#include "Messages.h"
#include "ContactHeader.h"
#include "ViaHeader.h"
#include "MaxForwardsHeader.h"
#include "CallIdHeader.h"
#include "CSeqHeader.h"
#include "ExpiresHeader.h"
#include "ContentLengthHeader.h"
#include "Header.h"
}


TEST_GROUP(MessageBuilderTestGroup)
{
    struct Message *m;
    void setup()
    {
        m = BuildRegisterMessage();
    }

    void teardown()
    {
        DestoryMessage(&m);
    }
};

TEST(MessageBuilderTestGroup, RequestLineTest)
{

    struct RequestLine *rl = MessageGetRequest(m);
    struct URI *uri = RequestLineGetUri(rl);
    
    STRCMP_EQUAL("sip", UriGetScheme(uri));
    STRCMP_EQUAL(PROXY_IPADDR, UriGetHost(uri));
}

TEST(MessageBuilderTestGroup, FromHeaderTest)
{
    struct ContactHeader *from = (struct ContactHeader *) MessageGetHeader(HEADER_NAME_FROM, m);
    STRCMP_EQUAL(HEADER_NAME_FROM, ContactHeaderGetName(from));
    STRCMP_EQUAL(USER_NAME, ContactHeaderGetDisplayName(from));

    struct URI *uri = ContactHeaderGetUri(from);
    STRCMP_EQUAL(USER_NAME, UriGetUser(uri));
    STRCMP_EQUAL(PROXY_IPADDR, UriGetHost(uri));
}

TEST(MessageBuilderTestGroup, ToHeaderTest)
{
    struct ContactHeader *to = (struct ContactHeader *) MessageGetHeader(HEADER_NAME_TO, m);
    STRCMP_EQUAL(HEADER_NAME_TO, ContactHeaderGetName(to));
    STRCMP_EQUAL(USER_NAME, ContactHeaderGetDisplayName(to));

    struct URI *uri = ContactHeaderGetUri(to);
    STRCMP_EQUAL(USER_NAME, UriGetUser(uri));
    STRCMP_EQUAL(PROXY_IPADDR, UriGetHost(uri));
}

TEST(MessageBuilderTestGroup, ViaHeaderTest)
{
    struct ViaHeader *via = (struct ViaHeader *) MessageGetHeader(HEADER_NAME_VIA, m);
    
    STRCMP_EQUAL(HEADER_NAME_VIA, ViaHeaderGetName(via));
    STRCMP_EQUAL(LOCAL_IPADDR, UriGetHost(ViaHeaderGetUri(via)));

    STRCMP_EQUAL("SIP/2.0/UDP", ViaHeaderGetTransport(via));
}

TEST(MessageBuilderTestGroup, ContactHeaderTest)
{
    struct ContactHeader *contact = (struct ContactHeader *) MessageGetHeader(HEADER_NAME_CONTACT, m);

    STRCMP_EQUAL(HEADER_NAME_CONTACT, ContactHeaderGetName(contact));
}

TEST(MessageBuilderTestGroup, MaxForwardHeaderTest)
{
    struct MaxForwardsHeader *mf = (struct MaxForwardsHeader *) MessageGetHeader(HEADER_NAME_MAX_FORWARDS, m);
    
    STRCMP_EQUAL(HEADER_NAME_MAX_FORWARDS, MaxForwardsGetName(mf));
}


TEST(MessageBuilderTestGroup, CallIdHeaderTest)
{
    struct CallIdHeader *id = (struct CallIdHeader *) MessageGetHeader(HEADER_NAME_CALLID, m);

    STRCMP_EQUAL(HEADER_NAME_CALLID, CallIdHeaderGetName(id));
}

TEST(MessageBuilderTestGroup, CSeqHeaerTest)
{
    struct CSeqHeader *cseq = (struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, m);
    
    STRCMP_EQUAL(HEADER_NAME_CSEQ, CSeqHeaderGetName(cseq));
}

TEST(MessageBuilderTestGroup, ExpiresHeaderTest)
{
    struct ExpiresHeader *e = (struct ExpiresHeader *) MessageGetHeader(HEADER_NAME_EXPIRES, m);
    
    STRCMP_EQUAL(HEADER_NAME_EXPIRES, ExpiresHeaderGetName(e));
}

TEST(MessageBuilderTestGroup, ContentLengthTest)
{
    struct ContentLengthHeader *c = (struct ContentLengthHeader *)MessageGetHeader(HEADER_NAME_CONTENT_LENGTH, m);

    STRCMP_EQUAL(HEADER_NAME_CONTENT_LENGTH, ContentLengthGetName(c));
}

TEST(MessageBuilderTestGroup, ToStringTest)
{
    char expected[2048] ="\
REGISTER sip:192.168.10.62 SIP/2.0\r\n\
Via:SIP/2.0/UDP 192.168.10.1:5060;rport;branch=z9hG4bK1500504766\r\n\
From:<sip:88001@192.168.10.62>;tag=1069855717\r\n\
To:<sip:88001@192.168.10.62>\r\n\
Call-ID:1626200011\r\n\
CSeq:1 REGISTER\r\n\
Contact:<sip:88001@192.168.10.1;line=6c451db26592505>\r\n\
Max-Forwards:70\r\n\
Expires:3600\r\n\
Content-Length:0";

    char result[2048] = {0};

    Message2String(result, m);

    STRCMP_EQUAL(expected, result)
}
