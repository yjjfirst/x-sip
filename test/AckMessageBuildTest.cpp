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
#include "DialogManager.h"
#include "WWW_AuthenticationHeader.h"
}

TEST_GROUP(AckMessageBuildTestGroup)
{
    struct UserAgent *ua;
    struct Dialog *dialog;
    MESSAGE *inviteMessage;
    void setup()
    {
        UT_PTR_SET(GenerateBranch, GenerateBranchMock);
        
        AccountInit();
        ua = CreateUserAgent(0);
        dialog = AddDialog(NULL_DIALOG_ID, ua);
        inviteMessage = DialogBuildInvite(dialog, (char *)"88002");
    }

    void teardown()
    {
        ClearAccountManager();
        DestroyMessage(&inviteMessage);
        DestroyUserAgent(&ua);
    }
};

TEST(AckMessageBuildTestGroup, BuildAckRequestWithinClientTransactionCallIdTest)
{
    MESSAGE *ack = BuildAckMessageWithinClientTransaction(inviteMessage);
    
    STRCMP_EQUAL(MessageGetCallId(inviteMessage), MessageGetCallId(ack));    

    DestroyMessage(&ack);
}

TEST(AckMessageBuildTestGroup, BuildAckRequestWithinClientTransactionRequestUriTest)
{
    MESSAGE *ack = BuildAckMessageWithinClientTransaction(inviteMessage);    
    struct RequestLine *rl = MessageGetRequestLine(ack);

    STRCMP_EQUAL("ACK", RequestLineGetMethodName(rl));
    CHECK_TRUE(UriMatched(RequestLineGetUri(MessageGetRequestLine(inviteMessage)), RequestLineGetUri(rl)));

    DestroyMessage(&ack);
}

TEST(AckMessageBuildTestGroup, BuildAckRequestWithinClientTransactionFromHeaderTest)
{
    MESSAGE *ack = BuildAckMessageWithinClientTransaction(inviteMessage);    

    CHECK_TRUE(ContactHeaderMatched((CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_FROM,inviteMessage), 
                                    (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_FROM,ack)));

    DestroyMessage(&ack);
}

TEST(AckMessageBuildTestGroup, BuildAckRequestWithClientTransactionToHeaderTest)
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

TEST(AckMessageBuildTestGroup, BuildAckRequestWithClientTransactionViaHeaderTest)
{
    MESSAGE *ack = BuildAckMessageWithinClientTransaction(inviteMessage);    

    CHECK_TRUE(ViaHeaderMatched((VIA_HEADER *)MessageGetHeader(HEADER_NAME_VIA, inviteMessage),
                                (VIA_HEADER *)MessageGetHeader(HEADER_NAME_VIA, ack)));

    DestroyMessage(&ack);
}

TEST(AckMessageBuildTestGroup, BuildAckRequestWithClientTransactionCseqHeaderTest)
{
    MESSAGE *ack = BuildAckMessageWithinClientTransaction(inviteMessage);    

    struct CSeqHeader *seq = (struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, ack);
    STRCMP_EQUAL(SIP_METHOD_NAME_ACK, CSeqHeaderGetMethod(seq));
    
    DestroyMessage(&ack);
}

TEST(AckMessageBuildTestGroup, AckMessageRequestLineTest)
{
    MESSAGE *ackMessage = BuildAckMessage(inviteMessage);
    struct RequestLine *requestLine = MessageGetRequestLine(ackMessage);
    URI *uri = RequestLineGetUri(requestLine);
    URI *inviteUri = RequestLineGetUri(MessageGetRequestLine(inviteMessage));

    STRCMP_EQUAL("ACK", RequestLineGetMethodName(requestLine));
    STRCMP_EQUAL(SIP_VERSION, RequestLineGetSipVersion(requestLine));
    CHECK_TRUE(UriMatched(uri, inviteUri));

    DestroyMessage(&ackMessage);
}

TEST(AckMessageBuildTestGroup, AckMessageCallIdTest)
{
    MESSAGE *ackMessage = BuildAckMessage(inviteMessage);
    
    STRCMP_EQUAL(MessageGetCallId(inviteMessage), MessageGetCallId(ackMessage));
    DestroyMessage(&ackMessage);
}

TEST(AckMessageBuildTestGroup, AckMessageFromTest)
{
    MESSAGE *ackMessage = BuildAckMessage(inviteMessage);
    CONTACT_HEADER *from = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_FROM, ackMessage);
    CONTACT_HEADER *inviteFrom = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_FROM, inviteMessage);

    CHECK_TRUE(ContactHeaderMatched(from, inviteFrom));
    
    DestroyMessage(&ackMessage);
}

TEST(AckMessageBuildTestGroup, AckMessageCSeqTest)
{
    MESSAGE *ackMessage = BuildAckMessage(inviteMessage);

    CHECK_EQUAL(MessageGetCSeqNumber(ackMessage),MessageGetCSeqNumber(inviteMessage));

    DestroyMessage(&ackMessage);
}

TEST(AckMessageBuildTestGroup, AckMessageViaTest)
{
    MESSAGE *ackMessage = BuildAckMessage(inviteMessage);
    VIA_HEADER *inviteVia = (VIA_HEADER *)MessageGetHeader(HEADER_NAME_VIA, inviteMessage);
    VIA_HEADER *ackVia = (VIA_HEADER *)MessageGetHeader(HEADER_NAME_VIA, ackMessage);

    CHECK_TRUE(ViaHeaderMatched(ackVia, inviteVia));
    DestroyMessage(&ackMessage);
}

