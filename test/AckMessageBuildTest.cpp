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
#include "Provision.h"
#include "TestingMessages.h"
}

TEST_GROUP(AckMessageBuildTestGroup)
{
    MESSAGE *inviteMessage;

    char proxy[64];
    int port;
    void setup()
    {
        char from[] = "88001";
        char to[] = "88002";
        strcpy(proxy, "192.168.10.62");
        port = 5060;
        
        UT_PTR_SET(GenerateBranch, GenerateBranchMock);        
        inviteMessage = BuildInviteMessage(from, to, proxy, port);
    }

    void teardown()
    {
        DestroyMessage(&inviteMessage);
    }
};

TEST(AckMessageBuildTestGroup, BuildAckRequestWithinClientTransactionCallIdTest)
{
    MESSAGE *ack = BuildAckMessageWithTag(inviteMessage, proxy, port,(char *) "");
    
    STRCMP_EQUAL(MessageGetCallId(inviteMessage), MessageGetCallId(ack));    

    DestroyMessage(&ack);
}

TEST(AckMessageBuildTestGroup, BuildAckRequestWithinClientTransactionRequestUriTest)
{
    MESSAGE *ack = BuildAckMessageWithTag(inviteMessage, proxy, port, (char *)"");    
    struct RequestLine *rl = MessageGetRequestLine(ack);

    STRCMP_EQUAL("ACK", RequestLineGetMethodName(rl));
    CHECK_TRUE(UriMatched(RequestLineGetUri(MessageGetRequestLine(inviteMessage)), RequestLineGetUri(rl)));

    DestroyMessage(&ack);
}

TEST(AckMessageBuildTestGroup, BuildAckRequestWithinClientTransactionFromHeaderTest)
{
    MESSAGE *ack = BuildAckMessageWithTag(inviteMessage, proxy, port, (char *)"");    

    CHECK_TRUE(ContactHeaderMatched((CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_FROM,inviteMessage), 
                                    (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_FROM,ack)));

    DestroyMessage(&ack);
}

TEST(AckMessageBuildTestGroup, BuildAckRequestWithClientTransactionToHeaderTest)
{
    MESSAGE *ack = BuildAckMessageWithTag(inviteMessage, proxy, port, (char *)"123456");    
    CONTACT_HEADER *toHeaderOfAck = 
        ContactHeaderDup((CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_TO, ack));

    STRCMP_EQUAL("123456",ContactHeaderGetParameter(toHeaderOfAck, (char *)"tag"));

    DestroyMessage(&ack);
    DestroyContactHeader((struct Header *)toHeaderOfAck);
}

TEST(AckMessageBuildTestGroup, BuildAckRequestWithClientTransactionViaHeaderTest)
{
    MESSAGE *ack = BuildAckMessageWithTag(inviteMessage, proxy, port, (char *)"");    

    CHECK_TRUE(ViaHeaderMatched((VIA_HEADER *)MessageGetHeader(HEADER_NAME_VIA, inviteMessage),
                                (VIA_HEADER *)MessageGetHeader(HEADER_NAME_VIA, ack)));

    DestroyMessage(&ack);
}

TEST(AckMessageBuildTestGroup, BuildAckRequestWithClientTransactionCseqHeaderTest)
{
    MESSAGE *ack = BuildAckMessageWithTag(inviteMessage, proxy, port, (char *)"");    

    struct CSeqHeader *seq = (struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, ack);
    STRCMP_EQUAL(SIP_METHOD_NAME_ACK, CSeqHeaderGetMethod(seq));
    
    DestroyMessage(&ack);
}

TEST(AckMessageBuildTestGroup, BuildAckRequestWithClientTransactionDestAddrTest)
{
    MESSAGE *ack = BuildAckMessageWithTag(inviteMessage, proxy, port, (char *)"");    

    STRCMP_EQUAL((char *)"192.168.10.62", GetMessageAddr(ack));
    
    DestroyMessage(&ack);
}

TEST(AckMessageBuildTestGroup, BuildAckRequestWithClientTransactionDestPortTest)
{
    MESSAGE *ack = BuildAckMessageWithTag(inviteMessage, proxy, port, (char *)"");    

    CHECK_EQUAL(port, GetMessagePort(ack));
    
    DestroyMessage(&ack);
}

TEST(AckMessageBuildTestGroup, AckMessageRequestLineTest)
{
    MESSAGE *ackMessage = BuildAckMessage(inviteMessage, (char *)"192.168.10.62", 5060);
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
    MESSAGE *ackMessage = BuildAckMessage(inviteMessage, (char *)"192.168.10.62", 5060);
    
    STRCMP_EQUAL(MessageGetCallId(inviteMessage), MessageGetCallId(ackMessage));
    DestroyMessage(&ackMessage);
}

TEST(AckMessageBuildTestGroup, AckMessageFromTest)
{
    MESSAGE *ackMessage = BuildAckMessage(inviteMessage, (char *)"192.168.10.62", 5060);
    CONTACT_HEADER *from = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_FROM, ackMessage);
    CONTACT_HEADER *inviteFrom = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_FROM, inviteMessage);

    CHECK_TRUE(ContactHeaderMatched(from, inviteFrom));
    
    DestroyMessage(&ackMessage);
}

TEST(AckMessageBuildTestGroup, AckMessageCSeqTest)
{
    MESSAGE *ackMessage = BuildAckMessage(inviteMessage, (char *)"192.168.10.62", 5060);

    CHECK_EQUAL(MessageGetCSeqNumber(ackMessage),MessageGetCSeqNumber(inviteMessage));

    DestroyMessage(&ackMessage);
}

TEST(AckMessageBuildTestGroup, AckMessageViaTest)
{
    MESSAGE *ackMessage = BuildAckMessage(inviteMessage, (char *)"192.168.10.62", 5060);
    VIA_HEADER *inviteVia = (VIA_HEADER *)MessageGetHeader(HEADER_NAME_VIA, inviteMessage);
    VIA_HEADER *ackVia = (VIA_HEADER *)MessageGetHeader(HEADER_NAME_VIA, ackMessage);

    CHECK_TRUE(ViaHeaderMatched(ackVia, inviteVia));
    DestroyMessage(&ackMessage);
}

