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

char user[] = "Martin Yang";
char host[] = "192.168.10.62";

TEST_GROUP(MessageBuilderTestGroup)
{
    struct Message *m;
    void setup()
    {
        m = BuildRegisterMessage(user, host);
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
    STRCMP_EQUAL(host, UriGetHost(uri));
}

TEST(MessageBuilderTestGroup, FromHeaderTest)
{
    struct ContactHeader *from = (struct ContactHeader *) MessageGetHeader(HEADER_NAME_FROM, m);
    STRCMP_EQUAL(HEADER_NAME_FROM, ContactHeaderGetName(from));
    STRCMP_EQUAL("Martin Yang", ContactHeaderGetDisplayName(from));

    struct URI *uri = ContactHeaderGetUri(from);
    STRCMP_EQUAL("Martin Yang", UriGetUser(uri));
    STRCMP_EQUAL("192.168.10.62", UriGetHost(uri));
}

TEST(MessageBuilderTestGroup, ToHeaderTest)
{
    struct ContactHeader *to = (struct ContactHeader *) MessageGetHeader(HEADER_NAME_TO, m);
    STRCMP_EQUAL(HEADER_NAME_TO, ContactHeaderGetName(to));
    STRCMP_EQUAL("Martin Yang", ContactHeaderGetDisplayName(to));

    struct URI *uri = ContactHeaderGetUri(to);
    STRCMP_EQUAL("Martin Yang", UriGetUser(uri));
    STRCMP_EQUAL("192.168.10.62", UriGetHost(uri));
}

TEST(MessageBuilderTestGroup, ViaHeaderTest)
{
    struct ViaHeader *via = (struct ViaHeader *) MessageGetHeader(HEADER_NAME_VIA, m);
    
    STRCMP_EQUAL(HEADER_NAME_VIA, ViaHeaderGetName(via));
    STRCMP_EQUAL("192.168.10.63", ViaHeaderGetUri(via));
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
    char expected[2048] = "REGISTER sip:Martin Yang@192.168.10.62:5060 SIP/2.0\r\n\
From:\"Martin Yang\"<sip:Martin Yang@192.168.10.62:5060>\r\n\
To:\"Martin Yang\"<sip:Martin Yang@192.168.10.62:5060>\r\n\
Via:SIP/2.0/UDP 192.168.10.63\r\n\
Contact:<sip:Martin Yang@192.168.10.62:5060>\r\n\
Max-Forwards:70\r\n\
Call-ID:1234567890\r\n\
CSeq:1826 REGISTER\r\n\
Expires:7200\r\n\
Content-Length:0\r\n";
    char result[2048] = {0};

    Message2String(result, m);

    STRCMP_EQUAL(expected, result)
}
