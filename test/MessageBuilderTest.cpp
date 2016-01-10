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
#include "Parameter.h"
#include "UserAgent.h"
#include "Dialog.h"
#include "Provision.h"
#include "TestingMessages.h"
}


TEST_GROUP(MessageBuilderTestGroup)
{
    struct UserAgent *ua;
    struct Message *m;
    struct Dialog *dialog;
    struct Message *inviteMessage;
    void setup()
    {
        ua = CreateUserAgent();
        UserAgentSetProxy(ua, GetProxy());
        UserAgentSetUserName(ua, GetUserName());

        dialog = CreateDialog(NULL, ua);

        DialogSetToUser(dialog, GetUserName());
        m = BuildBindingMessage(dialog);

        DialogSetToUser(dialog, (char *)"88002");
        inviteMessage = BuildInviteMessage(dialog);
    }

    void teardown()
    {
        DestoryMessage(&m);
        DestoryMessage(&inviteMessage);
        DestoryUserAgent(&ua);
    }
};

TEST(MessageBuilderTestGroup, MessageTypeTest)
{
    CHECK_EQUAL(MESSAGE_TYPE_REQUEST, MessageGetType(m));
    CHECK_EQUAL(MESSAGE_TYPE_REQUEST, MessageGetType(inviteMessage));

    struct Message *tmpMessage = CreateMessage();
    CHECK_EQUAL(MESSAGE_TYPE_NONE, MessageGetType(tmpMessage));
    DestoryMessage(&tmpMessage);

    tmpMessage = BuildAckMessage(dialog);
    CHECK_EQUAL(MESSAGE_TYPE_REQUEST, MessageGetType(tmpMessage));
    DestoryMessage(&tmpMessage);
}

TEST(MessageBuilderTestGroup, RequestLineTest)
{

    struct RequestLine *rl = MessageGetRequestLine(m);
    struct URI *uri = RequestLineGetUri(rl);
    
    STRCMP_EQUAL(URI_SCHEME_SIP, UriGetScheme(uri));
    STRCMP_EQUAL(GetProxy(), UriGetHost(uri));
    STRCMP_EQUAL(GetUserName(), UriGetUser(uri));
}

TEST(MessageBuilderTestGroup, FromHeaderTest)
{
    struct ContactHeader *from = (struct ContactHeader *) MessageGetHeader(HEADER_NAME_FROM, m);
    STRCMP_EQUAL(HEADER_NAME_FROM, ContactHeaderGetName(from));

    struct URI *uri = ContactHeaderGetUri(from);
    STRCMP_EQUAL(GetUserName(), UriGetUser(uri));
    STRCMP_EQUAL(GetProxy(), UriGetHost(uri));
}

TEST(MessageBuilderTestGroup, ToHeaderTest)
{
    struct ContactHeader *to = (struct ContactHeader *) MessageGetHeader(HEADER_NAME_TO, m);
    STRCMP_EQUAL(HEADER_NAME_TO, ContactHeaderGetName(to));

    struct URI *uri = ContactHeaderGetUri(to);
    STRCMP_EQUAL(GetUserName(), UriGetUser(uri));
    STRCMP_EQUAL(GetProxy(), UriGetHost(uri));
}

TEST(MessageBuilderTestGroup, ViaHeaderTest)
{
    struct ViaHeader *via = (struct ViaHeader *) MessageGetHeader(HEADER_NAME_VIA, m);
    
    MessageAddViaParameter(m, (char *)"rport", (char *)"");

    STRCMP_EQUAL(HEADER_NAME_VIA, ViaHeaderGetName(via));
    STRCMP_EQUAL(GetLocalIpAddr(), UriGetHost(ViaHeaderGetUri(via)));
    STRCMP_EQUAL("", ViaHeaderGetParameter(via, (char *)"rport"));
    STRCMP_EQUAL("z9hG4bK1491280923", ViaHeaderGetParameter(via, (char *)VIA_BRANCH_PARAMETER_NAME));

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

    STRCMP_EQUAL(HEADER_NAME_CONTENT_LENGTH, ContentLengthHeaderGetName(c));
}

TEST(MessageBuilderTestGroup, InviteMessageRequestLineTest)
{
    struct RequestLine *rl = MessageGetRequestLine(inviteMessage);
    STRCMP_EQUAL(SIP_METHOD_NAME_INVITE,RequestLineGetMethodName(rl));
    
    STRCMP_EQUAL("88002", UriGetUser(RequestLineGetUri(rl)));

}

TEST(MessageBuilderTestGroup, InviteMessageToHeaderTest)
{
    struct ContactHeader *to = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_TO, inviteMessage);
    struct URI *uri = ContactHeaderGetUri(to);

    STRCMP_EQUAL("88002", UriGetUser(uri));
}

TEST(MessageBuilderTestGroup, InviteMessageCseqHeaderTest)
{
    struct CSeqHeader *c = (struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, inviteMessage);
    STRCMP_EQUAL(SIP_METHOD_NAME_INVITE, CSeqHeaderGetMethod(c));
}

TEST(MessageBuilderTestGroup, InviteContentLengthTest)
{
    int contengLength = 436;

    MessageSetContentLength(inviteMessage, contengLength);
    CHECK_EQUAL(contengLength, ContentLengthHeaderGetLength(
                    (struct ContentLengthHeader *)MessageGetHeader(HEADER_NAME_CONTENT_LENGTH, inviteMessage)));

}

TEST(MessageBuilderTestGroup, AckMessageRequestLineTest)
{
    struct Message *ackMessage = BuildAckMessage(dialog);
    struct RequestLine *requestLine = MessageGetRequestLine(ackMessage);
    struct URI *uri = RequestLineGetUri(requestLine);
    struct URI *inviteUri = RequestLineGetUri(MessageGetRequestLine(inviteMessage));
        
    STRCMP_EQUAL("ACK", RequestLineGetMethodName(requestLine));
    STRCMP_EQUAL("SIP/2.0", RequestLineGetSipVersion(requestLine));
    CHECK_TRUE(UriMatched(uri, inviteUri));

    DestoryMessage(&ackMessage);
}

TEST(MessageBuilderTestGroup, AckMessageCallIdTest)
{
    struct Message *ackMessage = BuildAckMessage(dialog);
    
    STRCMP_EQUAL(MessageGetCallId(inviteMessage), MessageGetCallId(ackMessage));
    DestoryMessage(&ackMessage);
}

TEST(MessageBuilderTestGroup, AckMessageFromTest)
{
    struct Message *ackMessage = BuildAckMessage(dialog);
    struct ContactHeader *from = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_FROM, ackMessage);
    struct ContactHeader *inviteFrom = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_FROM, inviteMessage);
    CHECK_TRUE(ContactHeaderMatched(from, inviteFrom));

    DestoryMessage(&ackMessage);
}

TEST(MessageBuilderTestGroup, AckMessageCSeqTest)
{
    struct Message *ackMessage = BuildAckMessage(dialog);

    CHECK_EQUAL(MessageGetCSeqNumber(ackMessage),MessageGetCSeqNumber(inviteMessage));

    DestoryMessage(&ackMessage);
}

TEST(MessageBuilderTestGroup, AckMessageViaTest)
{
    struct Message *ackMessage = BuildAckMessage(dialog);
    struct ViaHeader *inviteVia = (struct ViaHeader *)MessageGetHeader(HEADER_NAME_VIA, inviteMessage);
    struct ViaHeader *ackVia = (struct ViaHeader *)MessageGetHeader(HEADER_NAME_VIA, ackMessage);

    CHECK_TRUE(ViaHeaderMatched(ackVia, inviteVia));
    DestoryMessage(&ackMessage);
}

IGNORE_TEST(MessageBuilderTestGroup, Build100TryingMessageTest)
{
    struct Message *invite = CreateMessage();
    ParseMessage((char *)INCOMMING_INVITE_MESSAGE, invite);

    struct Message *trying = Build100TryingMessage(invite);
    struct ContactHeader *inviteFrom = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_FROM, invite);
    struct ContactHeader *tryingFrom = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_FROM, trying);

    CHECK_TRUE(ContactHeaderMatched(inviteFrom, tryingFrom));

    DestoryMessage(&trying);
    DestoryMessage(&invite);
}
