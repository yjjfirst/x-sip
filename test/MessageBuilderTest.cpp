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
}

TEST_GROUP(MessageBuilderTestGroup)
{
    struct UserAgent *ua;
    struct Message *m;
    struct Dialog *dialog;
    struct Message *inviteMessage;
    void setup()
    {
        UT_PTR_SET(GenerateBranch, GenerateBranchMock);
        
        AccountInit();
        ua = CreateUserAgent(0);
        dialog = AddNewDialog(NULL_DIALOG_ID, ua);

        DialogSetToUser(dialog, GetUserName(0));
        m = BuildAddBindingMessage(dialog);

        DialogSetToUser(dialog, (char *)"88002");
        inviteMessage = BuildInviteMessage(dialog);
    }

    void teardown()
    {
        ClearAccountManager();
        DestroyMessage(&m);
        DestroyMessage(&inviteMessage);
        DestroyUserAgent(&ua);
    }
};

TEST(MessageBuilderTestGroup, MessageTypeTest)
{
    CHECK_EQUAL(MESSAGE_TYPE_REQUEST, MessageGetType(m));
    CHECK_EQUAL(MESSAGE_TYPE_REQUEST, MessageGetType(inviteMessage));

    struct Message *tmpMessage = CreateMessage();
    CHECK_EQUAL(MESSAGE_TYPE_NONE, MessageGetType(tmpMessage));
    DestroyMessage(&tmpMessage);

    tmpMessage = BuildAckMessage(dialog);
    CHECK_EQUAL(MESSAGE_TYPE_REQUEST, MessageGetType(tmpMessage));
    DestroyMessage(&tmpMessage);
}

TEST(MessageBuilderTestGroup, RequestLineTest)
{

    struct RequestLine *rl = MessageGetRequestLine(m);
    struct URI *uri = RequestLineGetUri(rl);
    
    STRCMP_EQUAL(URI_SCHEME_SIP, UriGetScheme(uri));
    STRCMP_EQUAL(GetProxy(0), UriGetHost(uri));
    STRCMP_EQUAL(GetUserName(0), UriGetUser(uri));
}

TEST(MessageBuilderTestGroup, FromHeaderTest)
{
    struct ContactHeader *from = (struct ContactHeader *) MessageGetHeader(HEADER_NAME_FROM, m);
    STRCMP_EQUAL(HEADER_NAME_FROM, ContactHeaderGetName(from));

    struct URI *uri = ContactHeaderGetUri(from);
    STRCMP_EQUAL(GetUserName(0), UriGetUser(uri));
    STRCMP_EQUAL(GetProxy(0), UriGetHost(uri));
}

TEST(MessageBuilderTestGroup, ToHeaderTest)
{
    struct ContactHeader *to = (struct ContactHeader *) MessageGetHeader(HEADER_NAME_TO, m);
    STRCMP_EQUAL(HEADER_NAME_TO, ContactHeaderGetName(to));

    struct URI *uri = ContactHeaderGetUri(to);
    STRCMP_EQUAL(GetUserName(0), UriGetUser(uri));
    STRCMP_EQUAL(GetProxy(0), UriGetHost(uri));
}

TEST(MessageBuilderTestGroup, ViaHeaderTest)
{
    struct ViaHeader *via = (struct ViaHeader *) MessageGetHeader(HEADER_NAME_VIA, m);
    char branch[64];
    
    GenerateBranch(branch);
    MessageAddViaParameter(m, (char *)"rport", (char *)"");

    STRCMP_EQUAL(HEADER_NAME_VIA, ViaHeaderGetName(via));
    STRCMP_EQUAL(GetLocalIpAddr(), UriGetHost(ViaHeaderGetUri(via)));
    STRCMP_EQUAL("", ViaHeaderGetParameter(via, (char *)"rport"));
    STRCMP_EQUAL(branch, ViaHeaderGetParameter(via, (char *)VIA_BRANCH_PARAMETER_NAME));

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
    STRCMP_EQUAL(SIP_VERSION, RequestLineGetSipVersion(requestLine));
    CHECK_TRUE(UriMatched(uri, inviteUri));

    DestroyMessage(&ackMessage);
}

TEST(MessageBuilderTestGroup, AckMessageCallIdTest)
{
    struct Message *ackMessage = BuildAckMessage(dialog);
    
    STRCMP_EQUAL(MessageGetCallId(inviteMessage), MessageGetCallId(ackMessage));
    DestroyMessage(&ackMessage);
}

TEST(MessageBuilderTestGroup, AckMessageFromTest)
{
    struct Message *ackMessage = BuildAckMessage(dialog);
    struct ContactHeader *from = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_FROM, ackMessage);
    struct ContactHeader *inviteFrom = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_FROM, inviteMessage);
    CHECK_TRUE(ContactHeaderMatched(from, inviteFrom));

    DestroyMessage(&ackMessage);
}

TEST(MessageBuilderTestGroup, AckMessageCSeqTest)
{
    struct Message *ackMessage = BuildAckMessage(dialog);

    CHECK_EQUAL(MessageGetCSeqNumber(ackMessage),MessageGetCSeqNumber(inviteMessage));

    DestroyMessage(&ackMessage);
}

TEST(MessageBuilderTestGroup, AckMessageViaTest)
{
    struct Message *ackMessage = BuildAckMessage(dialog);
    struct ViaHeader *inviteVia = (struct ViaHeader *)MessageGetHeader(HEADER_NAME_VIA, inviteMessage);
    struct ViaHeader *ackVia = (struct ViaHeader *)MessageGetHeader(HEADER_NAME_VIA, ackMessage);

    CHECK_TRUE(ViaHeaderMatched(ackVia, inviteVia));
    DestroyMessage(&ackMessage);
}

TEST(MessageBuilderTestGroup, TryingMessageStatusLineTest)
{
    struct Message *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);

    struct Message *trying = BuildTryingMessage(invite);
    struct StatusLine *status = MessageGetStatusLine(trying);
    
    CHECK_TRUE(status != NULL);
    STRCMP_EQUAL(SIP_VERSION, StatusLineGetSipVersion(status));
    CHECK_EQUAL(100, StatusLineGetStatusCode(status));
    STRCMP_EQUAL("Trying", StatusLineGetReasonPhrase(status));

    DestroyMessage(&trying);
    DestroyMessage(&invite);
}

TEST(MessageBuilderTestGroup, TryingMessageFromHeaderTest)
{
    struct Message *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);

    struct Message *trying = BuildTryingMessage(invite);
    struct ContactHeader *inviteFrom = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_FROM, invite);
    struct ContactHeader *tryingFrom = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_FROM, trying);

    CHECK_TRUE(ContactHeaderMatched(inviteFrom, tryingFrom));

    DestroyMessage(&trying);
    DestroyMessage(&invite);
}

TEST(MessageBuilderTestGroup, TryingMessageToWithTagTest)
{
    struct Message *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE_WITH_TO_TAG, invite);
    
    struct Message *trying = BuildTryingMessage(invite);

    struct ContactHeader *inviteTo = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_TO, invite);
    struct ContactHeader *tryingTo = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_TO, trying);

    CHECK_TRUE(ContactHeaderMatched(inviteTo, tryingTo));

    DestroyMessage(&trying);
    DestroyMessage(&invite);    
}

TEST(MessageBuilderTestGroup, TryingMessageToWithNoTagTest)
{
    struct Message *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    
    struct Message *trying = BuildTryingMessage(invite);

    struct ContactHeader *inviteTo = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_TO, invite);
    struct ContactHeader *tryingTo = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_TO, trying);

    struct URI *inviteToUri = ContactHeaderGetUri(inviteTo);
    struct URI *tryingToUri = ContactHeaderGetUri(tryingTo);

    CHECK_FALSE(ContactHeaderMatched(inviteTo, tryingTo));
    CHECK_TRUE(UriMatched(inviteToUri, tryingToUri));

    DestroyMessage(&trying);
    DestroyMessage(&invite);    
}

TEST(MessageBuilderTestGroup, TryingMessageCallIdTest)
{
    struct Message *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    struct Message *trying = BuildTryingMessage(invite);

    struct CallIdHeader *inviteCallId = (struct CallIdHeader *)MessageGetHeader(HEADER_NAME_CALLID, invite);
    struct CallIdHeader *tryingCallId = (struct CallIdHeader *)MessageGetHeader(HEADER_NAME_CALLID, trying);
    
    CHECK_TRUE(CallIdHeaderMatched(inviteCallId, tryingCallId));
    
    DestroyMessage(&trying);
    DestroyMessage(&invite);    
}

TEST(MessageBuilderTestGroup, TryingMessageCSeqTest)
{
    struct Message *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    struct Message *trying = BuildTryingMessage(invite);
    
    struct CSeqHeader *inviteCSeq = (struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, invite);
    struct CSeqHeader *tryingCSeq = (struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, trying);

    CHECK_TRUE(CSeqHeadersMatched(inviteCSeq, tryingCSeq));

    DestroyMessage(&trying);
    DestroyMessage(&invite);
}

TEST(MessageBuilderTestGroup, TryingMessageViaTest)
{
    struct Message *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    struct Message *trying = BuildTryingMessage(invite);

    struct ViaHeader *inviteVia = (struct ViaHeader *)MessageGetHeader(HEADER_NAME_VIA, invite);
    struct ViaHeader *tryingVia = (struct ViaHeader *)MessageGetHeader(HEADER_NAME_VIA, trying);
    
    CHECK_TRUE(ViaHeaderMatched(inviteVia, tryingVia));

    DestroyMessage(&trying);
    DestroyMessage(&invite);    
}

TEST(MessageBuilderTestGroup, RingingMessageStatusLineTest)
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

TEST(MessageBuilderTestGroup, OKMessageStatusLineTest)
{
    struct Message *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    struct Message *ok = Build200OKMessage(invite);
    
    struct StatusLine *sl = MessageGetStatusLine(ok);

    CHECK_EQUAL(200, StatusLineGetStatusCode(sl));
    STRCMP_EQUAL("OK", StatusLineGetReasonPhrase(sl));

    DestroyMessage(&invite);
    DestroyMessage(&ok);
}

TEST(MessageBuilderTestGroup,OKMessageContactHeaderTest)
{
    struct Message *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    struct Message *ok = Build200OKMessage(invite);
    struct ContactHeader *c = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_CONTACT, ok);
    struct URI *uri = ContactHeaderGetUri(c);
    
    CHECK_TRUE(c != NULL);
    STRCMP_EQUAL(UriGetScheme(uri), URI_SCHEME_SIP);
    STRCMP_EQUAL(GetLocalIpAddr(), UriGetHost(uri));
    CHECK_EQUAL(LOCAL_PORT, UriGetPort(uri));

    DestroyMessage(&invite);
    DestroyMessage(&ok);
}

TEST(MessageBuilderTestGroup, 301MessageStatueLineTest)
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

struct URI *DialogGetRemoteUriMock(struct Dialog *dialog)
{
    struct URI *uri = CreateEmptyUri();
    ParseUri((char *)"sip:abcd@192.168.10.123:tag=1234", &uri);
    return uri;
}

char *DialogGetRemoteTagMock(struct Dialog *dialog)
{
    return (char *)"123456abcde";
}

TEST(MessageBuilderTestGroup, ByeMessageToHeaderTest)
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

struct URI *dialog_remote_target;
struct URI *DialogGetRemoteTargetMock(struct Dialog *dialog)
{
    if (dialog_remote_target == NULL) {
        dialog_remote_target = CreateEmptyUri();
        ParseUri((char *)"sip:8800222@192.168.10.62", &dialog_remote_target);
    }
    return dialog_remote_target;
}

TEST(MessageBuilderTestGroup, ByeMessageRequestLineTest)
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

TEST(MessageBuilderTestGroup, BuildAckRequestWithinClientTransactionCallIdTest)
{
    struct Message *ack = BuildAckMessageWithinClientTransaction(inviteMessage);
    
    STRCMP_EQUAL(MessageGetCallId(inviteMessage), MessageGetCallId(ack));    

    DestroyMessage(&ack);
}

TEST(MessageBuilderTestGroup, BuildAckRequestWithinClientTransactionRequestUriTest)
{
    struct Message *ack = BuildAckMessageWithinClientTransaction(inviteMessage);    
    struct RequestLine *rl = MessageGetRequestLine(ack);

    STRCMP_EQUAL("ACK", RequestLineGetMethodName(rl));
    CHECK_TRUE(UriMatched(RequestLineGetUri(MessageGetRequestLine(inviteMessage)), RequestLineGetUri(rl)));

    DestroyMessage(&ack);
}

TEST(MessageBuilderTestGroup, BuildAckRequestWithinClientTransactionFromHeaderTest)
{
    struct Message *ack = BuildAckMessageWithinClientTransaction(inviteMessage);    

    CHECK_TRUE(ContactHeaderMatched((struct ContactHeader *)MessageGetHeader(HEADER_NAME_FROM,inviteMessage), 
                                    (struct ContactHeader *)MessageGetHeader(HEADER_NAME_FROM,ack)));

    DestroyMessage(&ack);
}

TEST(MessageBuilderTestGroup, BuildAckRequestWithClientTransactionToHeaderTest)
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

TEST(MessageBuilderTestGroup, BuildAckRequestWithClientTransactionViaHeaderTest)
{
    struct Message *ack = BuildAckMessageWithinClientTransaction(inviteMessage);    

    CHECK_TRUE(ViaHeaderMatched((struct ViaHeader *)MessageGetHeader(HEADER_NAME_VIA, inviteMessage),
                                (struct ViaHeader *)MessageGetHeader(HEADER_NAME_VIA, ack)));

    DestroyMessage(&ack);
}

TEST(MessageBuilderTestGroup, BuildAckRequestWithClientTransactionCseqHeaderTest)
{
    struct Message *ack = BuildAckMessageWithinClientTransaction(inviteMessage);    

    struct CSeqHeader *seq = (struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, ack);
    STRCMP_EQUAL(SIP_METHOD_NAME_ACK, CSeqHeaderGetMethod(seq));
    
    DestroyMessage(&ack);
}

TEST(MessageBuilderTestGroup, BindingsRequestLineTest)
{
    struct RequestLine *rl = MessageGetRequestLine(m);
    STRCMP_EQUAL("REGISTER", RequestLineGetMethodName(rl));
    STRCMP_EQUAL("SIP/2.0", RequestLineGetSipVersion(rl));

    struct URI *uri = RequestLineGetUri(rl);
    STRCMP_EQUAL(URI_SCHEME_SIP,  UriGetScheme(uri));
    STRCMP_EQUAL(GetProxy(0), UriGetHost(uri));
}

TEST(MessageBuilderTestGroup, BindingsToHeaderTest)
{
    struct ContactHeader *to = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_TO, m); 
    struct URI *uri = ContactHeaderGetUri(to);

    STRCMP_EQUAL(GetUserName(0), UriGetUser(uri));
}

TEST(MessageBuilderTestGroup, BindingsFromHeaderTest)
{
    struct ContactHeader *from = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_FROM, m); 
    struct URI *uri = ContactHeaderGetUri(from);

    STRCMP_EQUAL(GetUserName(0), UriGetUser(uri));
}

TEST(MessageBuilderTestGroup, BindingsContactHeaderTest)
{
    struct ContactHeader *contact = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_CONTACT, m); 
    struct URI *uri = ContactHeaderGetUri(contact);

    STRCMP_EQUAL(GetUserName(0), UriGetUser(uri));
}

TEST(MessageBuilderTestGroup, UnbindingMessage)
{
    struct Message *remove = BuildRemoveBindingMessage(dialog);
    struct ExpiresHeader *e = (struct ExpiresHeader *)MessageGetHeader(HEADER_NAME_EXPIRES, remove);
    
    CHECK_EQUAL(0, ExpiresHeaderGetExpires(e));
    DestroyMessage(&remove);
}
