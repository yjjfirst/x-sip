#include "CppUTest/TestHarness.h"
#include "Mock.h"

extern "C" {
#include <stdio.h>
#include <string.h>

#include "URI.h"
#include "RequestLine.h"
#include "StatusLine.h"
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
#include "UserAgentManager.h"
#include "AccountManager.h"
#include "WWW_AuthenticationHeader.h"
}

TEST_GROUP(InviteMessageBuildTestGroup)
{
    struct UserAgent *ua;
    struct Dialog *dialog;
    MESSAGE *inviteMessage;
    void setup()
    {
        UT_PTR_SET(GenerateBranch, GenerateBranchMock);
        
        AccountInit();
        ua = CreateUserAgent(0);
        dialog = AddNewDialog(NULL_DIALOG_ID, ua);
        inviteMessage = BuildInviteMessage(dialog , (char *)"88002");
    }

    void teardown()
    {
        ClearAccountManager();
        DestroyMessage(&inviteMessage);
        DestroyUserAgent(&ua);
    }
};

TEST(InviteMessageBuildTestGroup, InviteMessageRequestLineTest)
{
    struct RequestLine *rl = MessageGetRequestLine(inviteMessage);

    STRCMP_EQUAL(SIP_METHOD_NAME_INVITE,RequestLineGetMethodName(rl));
    STRCMP_EQUAL("88002", UriGetUser(RequestLineGetUri(rl)));
}

TEST(InviteMessageBuildTestGroup, InviteMessageToHeaderTest)
{
    CONTACT_HEADER *to = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_TO, inviteMessage);
    URI *uri = ContactHeaderGetUri(to);

    STRCMP_EQUAL("88002", UriGetUser(uri));
}

TEST(InviteMessageBuildTestGroup, InviteMessageCseqHeaderTest)
{
    struct CSeqHeader *c = (struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, inviteMessage);
    STRCMP_EQUAL(SIP_METHOD_NAME_INVITE, CSeqHeaderGetMethod(c));
}

TEST(InviteMessageBuildTestGroup, InviteContentLengthTest)
{
    int contengLength = 436;

    MessageSetContentLength(inviteMessage, contengLength);
    CHECK_EQUAL(contengLength, ContentLengthHeaderGetLength(
                    (struct ContentLengthHeader *)MessageGetHeader(HEADER_NAME_CONTENT_LENGTH, inviteMessage)));

}

TEST(InviteMessageBuildTestGroup, AckMessageRequestLineTest)
{
    MESSAGE *ackMessage = BuildAckMessage(dialog);
    struct RequestLine *requestLine = MessageGetRequestLine(ackMessage);
    URI *uri = RequestLineGetUri(requestLine);
    URI *inviteUri = RequestLineGetUri(MessageGetRequestLine(inviteMessage));

    STRCMP_EQUAL("ACK", RequestLineGetMethodName(requestLine));
    STRCMP_EQUAL(SIP_VERSION, RequestLineGetSipVersion(requestLine));
    CHECK_TRUE(UriMatched(uri, inviteUri));

    DestroyMessage(&ackMessage);
}

TEST(InviteMessageBuildTestGroup, AckMessageCallIdTest)
{
    MESSAGE *ackMessage = BuildAckMessage(dialog);
    
    STRCMP_EQUAL(MessageGetCallId(inviteMessage), MessageGetCallId(ackMessage));
    DestroyMessage(&ackMessage);
}

TEST(InviteMessageBuildTestGroup, AckMessageFromTest)
{
    MESSAGE *ackMessage = BuildAckMessage(dialog);
    CONTACT_HEADER *from = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_FROM, ackMessage);
    CONTACT_HEADER *inviteFrom = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_FROM, inviteMessage);

    CHECK_TRUE(ContactHeaderMatched(from, inviteFrom));
    
    DestroyMessage(&ackMessage);
}

TEST(InviteMessageBuildTestGroup, AckMessageCSeqTest)
{
    MESSAGE *ackMessage = BuildAckMessage(dialog);

    CHECK_EQUAL(MessageGetCSeqNumber(ackMessage),MessageGetCSeqNumber(inviteMessage));

    DestroyMessage(&ackMessage);
}

TEST(InviteMessageBuildTestGroup, AckMessageViaTest)
{
    MESSAGE *ackMessage = BuildAckMessage(dialog);
    VIA_HEADER *inviteVia = (VIA_HEADER *)MessageGetHeader(HEADER_NAME_VIA, inviteMessage);
    VIA_HEADER *ackVia = (VIA_HEADER *)MessageGetHeader(HEADER_NAME_VIA, ackMessage);

    CHECK_TRUE(ViaHeaderMatched(ackVia, inviteVia));
    DestroyMessage(&ackMessage);
}

TEST(InviteMessageBuildTestGroup, RingingMessageStatusLineTest)
{
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    MESSAGE *ringing = BuildRingingMessage(invite);
    
    struct StatusLine *sl = MessageGetStatusLine(ringing);

    CHECK_EQUAL(180, StatusLineGetStatusCode(sl));
    STRCMP_EQUAL("Ringing", StatusLineGetReasonPhrase(sl));

    DestroyMessage(&invite);
    DestroyMessage(&ringing);
}

TEST(InviteMessageBuildTestGroup, OKMessageStatusLineTest)
{
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    MESSAGE *ok = Build200OkMessage(invite);    
    struct StatusLine *sl = MessageGetStatusLine(ok);

    CHECK_EQUAL(200, StatusLineGetStatusCode(sl));
    STRCMP_EQUAL("OK", StatusLineGetReasonPhrase(sl));

    DestroyMessage(&invite);
    DestroyMessage(&ok);
}

TEST(InviteMessageBuildTestGroup,OKMessageContactHeaderTest)
{
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    MESSAGE *ok = Build200OkMessage(invite);
    CONTACT_HEADER *c = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_CONTACT, ok);
    URI *uri = ContactHeaderGetUri(c);
    
    CHECK_TRUE(c != NULL);
    STRCMP_EQUAL(UriGetScheme(uri), URI_SCHEME_SIP);
    STRCMP_EQUAL(GetLocalIpAddr(), UriGetHost(uri));
    CHECK_EQUAL(LOCAL_PORT, UriGetPort(uri));

    DestroyMessage(&invite);
    DestroyMessage(&ok);
}

TEST(InviteMessageBuildTestGroup, 301MessageStatueLineTest)
{
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    MESSAGE *moved = Build301Message(invite);
    
    struct StatusLine *sl = MessageGetStatusLine(moved);

    CHECK_EQUAL(301, StatusLineGetStatusCode(sl));
    STRCMP_EQUAL(REASON_PHRASE_MOVED_PERMANENTLY, StatusLineGetReasonPhrase(sl));

    DestroyMessage(&invite);
    DestroyMessage(&moved);

}

TEST(InviteMessageBuildTestGroup, ByeMessageToHeaderTest)
{
    MESSAGE *bye = BuildByeMessage(dialog);
    CONTACT_HEADER *to = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_TO, bye);
    URI *uri = ContactHeaderGetUri(to);
    URI *remoteUri = DialogGetRemoteUri(dialog);

    CHECK_TRUE(UriMatched(uri, remoteUri));
    STRCMP_EQUAL(DialogGetRemoteTag(dialog), ContactHeaderGetParameter(to, HEADER_PARAMETER_NAME_TAG));
    
    DestroyMessage(&bye);
}


TEST(InviteMessageBuildTestGroup, ByeMessageRequestLineTest)
{
    MESSAGE *bye = BuildByeMessage(dialog);
    struct RequestLine *rl = MessageGetRequestLine(bye);
    URI *uri = RequestLineGetUri(rl);
    URI *remoteUri = DialogGetRemoteUri(dialog);
    
    STRCMP_EQUAL(SIP_METHOD_NAME_BYE, RequestLineGetMethodName(rl));
    STRCMP_EQUAL(SIP_VERSION, RequestLineGetSipVersion(rl));
    CHECK_TRUE(UriMatched(remoteUri, uri));

    DestroyMessage(&bye);
}

TEST(InviteMessageBuildTestGroup, BuildAckRequestWithinClientTransactionCallIdTest)
{
    MESSAGE *ack = BuildAckMessageWithinClientTransaction(inviteMessage);
    
    STRCMP_EQUAL(MessageGetCallId(inviteMessage), MessageGetCallId(ack));    

    DestroyMessage(&ack);
}

TEST(InviteMessageBuildTestGroup, BuildAckRequestWithinClientTransactionRequestUriTest)
{
    MESSAGE *ack = BuildAckMessageWithinClientTransaction(inviteMessage);    
    struct RequestLine *rl = MessageGetRequestLine(ack);

    STRCMP_EQUAL("ACK", RequestLineGetMethodName(rl));
    CHECK_TRUE(UriMatched(RequestLineGetUri(MessageGetRequestLine(inviteMessage)), RequestLineGetUri(rl)));

    DestroyMessage(&ack);
}

TEST(InviteMessageBuildTestGroup, BuildAckRequestWithinClientTransactionFromHeaderTest)
{
    MESSAGE *ack = BuildAckMessageWithinClientTransaction(inviteMessage);    

    CHECK_TRUE(ContactHeaderMatched((CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_FROM,inviteMessage), 
                                    (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_FROM,ack)));

    DestroyMessage(&ack);
}

TEST(InviteMessageBuildTestGroup, BuildAckRequestWithClientTransactionToHeaderTest)
{
    MESSAGE *ack = BuildAckMessageWithinClientTransaction(inviteMessage);    
    CONTACT_HEADER *toHeaderOfAck = 
        ContactHeaderDup((CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_TO, ack));

    ContactHeaderRemoveParameters(toHeaderOfAck);
    CHECK_TRUE(ContactHeaderMatched((CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_TO,inviteMessage), 
                                    toHeaderOfAck));

    DestroyMessage(&ack);
    DestroyContactHeader((struct Header *)toHeaderOfAck);
}

TEST(InviteMessageBuildTestGroup, BuildAckRequestWithClientTransactionViaHeaderTest)
{
    MESSAGE *ack = BuildAckMessageWithinClientTransaction(inviteMessage);    

    CHECK_TRUE(ViaHeaderMatched((VIA_HEADER *)MessageGetHeader(HEADER_NAME_VIA, inviteMessage),
                                (VIA_HEADER *)MessageGetHeader(HEADER_NAME_VIA, ack)));

    DestroyMessage(&ack);
}

TEST(InviteMessageBuildTestGroup, BuildAckRequestWithClientTransactionCseqHeaderTest)
{
    MESSAGE *ack = BuildAckMessageWithinClientTransaction(inviteMessage);    

    struct CSeqHeader *seq = (struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, ack);
    STRCMP_EQUAL(SIP_METHOD_NAME_ACK, CSeqHeaderGetMethod(seq));
    
    DestroyMessage(&ack);
}
