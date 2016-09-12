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
    struct Message *inviteMessage;
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
    struct ContactHeader *to = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_TO, inviteMessage);
    struct URI *uri = ContactHeaderGetUri(to);

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
    struct Message *ackMessage = BuildAckMessage(dialog);
    struct RequestLine *requestLine = MessageGetRequestLine(ackMessage);
    struct URI *uri = RequestLineGetUri(requestLine);
    struct URI *inviteUri = RequestLineGetUri(MessageGetRequestLine(inviteMessage));

    STRCMP_EQUAL("ACK", RequestLineGetMethodName(requestLine));
    STRCMP_EQUAL(SIP_VERSION, RequestLineGetSipVersion(requestLine));
    CHECK_TRUE(UriMatched(uri, inviteUri));

    DestroyMessage(&ackMessage);
}

TEST(InviteMessageBuildTestGroup, AckMessageCallIdTest)
{
    struct Message *ackMessage = BuildAckMessage(dialog);
    
    STRCMP_EQUAL(MessageGetCallId(inviteMessage), MessageGetCallId(ackMessage));
    DestroyMessage(&ackMessage);
}

TEST(InviteMessageBuildTestGroup, AckMessageFromTest)
{
    struct Message *ackMessage = BuildAckMessage(dialog);
    struct ContactHeader *from = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_FROM, ackMessage);
    struct ContactHeader *inviteFrom = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_FROM, inviteMessage);

    CHECK_TRUE(ContactHeaderMatched(from, inviteFrom));
    
    DestroyMessage(&ackMessage);
}

TEST(InviteMessageBuildTestGroup, AckMessageCSeqTest)
{
    struct Message *ackMessage = BuildAckMessage(dialog);

    CHECK_EQUAL(MessageGetCSeqNumber(ackMessage),MessageGetCSeqNumber(inviteMessage));

    DestroyMessage(&ackMessage);
}

TEST(InviteMessageBuildTestGroup, AckMessageViaTest)
{
    struct Message *ackMessage = BuildAckMessage(dialog);
    struct ViaHeader *inviteVia = (struct ViaHeader *)MessageGetHeader(HEADER_NAME_VIA, inviteMessage);
    struct ViaHeader *ackVia = (struct ViaHeader *)MessageGetHeader(HEADER_NAME_VIA, ackMessage);

    CHECK_TRUE(ViaHeaderMatched(ackVia, inviteVia));
    DestroyMessage(&ackMessage);
}

TEST(InviteMessageBuildTestGroup, RingingMessageStatusLineTest)
{
    struct Message *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    struct Message *ringing = BuildRingingMessage(invite);
    
    struct StatusLine *sl = MessageGetStatusLine(ringing);

    CHECK_EQUAL(180, StatusLineGetStatusCode(sl));
    STRCMP_EQUAL("Ringing", StatusLineGetReasonPhrase(sl));

    DestroyMessage(&invite);
    DestroyMessage(&ringing);
}

TEST(InviteMessageBuildTestGroup, OKMessageStatusLineTest)
{
    struct Message *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    struct Message *ok = Build200OkMessage(invite);    
    struct StatusLine *sl = MessageGetStatusLine(ok);

    CHECK_EQUAL(200, StatusLineGetStatusCode(sl));
    STRCMP_EQUAL("OK", StatusLineGetReasonPhrase(sl));

    DestroyMessage(&invite);
    DestroyMessage(&ok);
}

TEST(InviteMessageBuildTestGroup,OKMessageContactHeaderTest)
{
    struct Message *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    struct Message *ok = Build200OkMessage(invite);
    struct ContactHeader *c = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_CONTACT, ok);
    struct URI *uri = ContactHeaderGetUri(c);
    
    CHECK_TRUE(c != NULL);
    STRCMP_EQUAL(UriGetScheme(uri), URI_SCHEME_SIP);
    STRCMP_EQUAL(GetLocalIpAddr(), UriGetHost(uri));
    CHECK_EQUAL(LOCAL_PORT, UriGetPort(uri));

    DestroyMessage(&invite);
    DestroyMessage(&ok);
}

TEST(InviteMessageBuildTestGroup, 301MessageStatueLineTest)
{
    struct Message *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    struct Message *moved = Build301Message(invite);
    
    struct StatusLine *sl = MessageGetStatusLine(moved);

    CHECK_EQUAL(301, StatusLineGetStatusCode(sl));
    STRCMP_EQUAL(REASON_PHRASE_MOVED_PERMANENTLY, StatusLineGetReasonPhrase(sl));

    DestroyMessage(&invite);
    DestroyMessage(&moved);

}

static struct URI *DialogGetRemoteUriMock(struct Dialog *dialog)
{
    struct URI *uri = CreateEmptyUri();
    ParseUri((char *)"sip:abcd@192.168.10.123:tag=1234", &uri);
    return uri;
}

static char *DialogGetRemoteTagMock(struct Dialog *dialog)
{
    return (char *)"123456abcde";
}

TEST(InviteMessageBuildTestGroup, ByeMessageToHeaderTest)
{
    UT_PTR_SET(DialogGetRemoteUri, DialogGetRemoteUriMock);
    UT_PTR_SET(DialogGetRemoteTag, DialogGetRemoteTagMock);

    struct Message *bye = BuildByeMessage(dialog);
    struct ContactHeader *to = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_TO, bye);
    struct URI *uri = ContactHeaderGetUri(to);
    struct URI *remoteUri = DialogGetRemoteUri(dialog);
    
    CHECK_TRUE(UriMatched(uri, remoteUri));
    STRCMP_EQUAL(DialogGetRemoteTag(dialog), ContactHeaderGetParameter(to, HEADER_PARAMETER_NAME_TAG));

    DestroyUri(&remoteUri);
    DestroyMessage(&bye);
}

static struct URI *dialog_remote_target;
static struct URI *DialogGetRemoteTargetMock(struct Dialog *dialog)
{
    if (dialog_remote_target == NULL) {
        dialog_remote_target = CreateEmptyUri();
        ParseUri((char *)"sip:8800222@192.168.10.62", &dialog_remote_target);
    }
    return dialog_remote_target;
}

TEST(InviteMessageBuildTestGroup, ByeMessageRequestLineTest)
{
    UT_PTR_SET(DialogGetRemoteTarget, DialogGetRemoteTargetMock);

    struct Message *bye = BuildByeMessage(dialog);
    struct RequestLine *rl = MessageGetRequestLine(bye);
    struct URI *uri = RequestLineGetUri(rl);
    struct URI *remoteTarget = DialogGetRemoteTarget(dialog);
    
    STRCMP_EQUAL(SIP_METHOD_NAME_BYE, RequestLineGetMethodName(rl));
    STRCMP_EQUAL(SIP_VERSION, RequestLineGetSipVersion(rl));
    CHECK_TRUE(UriMatched(remoteTarget, uri));

    DestroyUri(&remoteTarget);
    DestroyMessage(&bye);

}

TEST(InviteMessageBuildTestGroup, BuildAckRequestWithinClientTransactionCallIdTest)
{
    struct Message *ack = BuildAckMessageWithinClientTransaction(inviteMessage);
    
    STRCMP_EQUAL(MessageGetCallId(inviteMessage), MessageGetCallId(ack));    

    DestroyMessage(&ack);
}

TEST(InviteMessageBuildTestGroup, BuildAckRequestWithinClientTransactionRequestUriTest)
{
    struct Message *ack = BuildAckMessageWithinClientTransaction(inviteMessage);    
    struct RequestLine *rl = MessageGetRequestLine(ack);

    STRCMP_EQUAL("ACK", RequestLineGetMethodName(rl));
    CHECK_TRUE(UriMatched(RequestLineGetUri(MessageGetRequestLine(inviteMessage)), RequestLineGetUri(rl)));

    DestroyMessage(&ack);
}

TEST(InviteMessageBuildTestGroup, BuildAckRequestWithinClientTransactionFromHeaderTest)
{
    struct Message *ack = BuildAckMessageWithinClientTransaction(inviteMessage);    

    CHECK_TRUE(ContactHeaderMatched((struct ContactHeader *)MessageGetHeader(HEADER_NAME_FROM,inviteMessage), 
                                    (struct ContactHeader *)MessageGetHeader(HEADER_NAME_FROM,ack)));

    DestroyMessage(&ack);
}

TEST(InviteMessageBuildTestGroup, BuildAckRequestWithClientTransactionToHeaderTest)
{
    struct Message *ack = BuildAckMessageWithinClientTransaction(inviteMessage);    
    struct ContactHeader *toHeaderOfAck = 
        ContactHeaderDup((struct ContactHeader *)MessageGetHeader(HEADER_NAME_TO, ack));

    ContactHeaderRemoveParameters(toHeaderOfAck);
    CHECK_TRUE(ContactHeaderMatched((struct ContactHeader *)MessageGetHeader(HEADER_NAME_TO,inviteMessage), 
                                    toHeaderOfAck));

    DestroyMessage(&ack);
    DestroyContactHeader((struct Header *)toHeaderOfAck);
}

TEST(InviteMessageBuildTestGroup, BuildAckRequestWithClientTransactionViaHeaderTest)
{
    struct Message *ack = BuildAckMessageWithinClientTransaction(inviteMessage);    

    CHECK_TRUE(ViaHeaderMatched((struct ViaHeader *)MessageGetHeader(HEADER_NAME_VIA, inviteMessage),
                                (struct ViaHeader *)MessageGetHeader(HEADER_NAME_VIA, ack)));

    DestroyMessage(&ack);
}

TEST(InviteMessageBuildTestGroup, BuildAckRequestWithClientTransactionCseqHeaderTest)
{
    struct Message *ack = BuildAckMessageWithinClientTransaction(inviteMessage);    

    struct CSeqHeader *seq = (struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, ack);
    STRCMP_EQUAL(SIP_METHOD_NAME_ACK, CSeqHeaderGetMethod(seq));
    
    DestroyMessage(&ack);
}
