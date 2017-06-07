#include "CppUTest/TestHarness.h"
#include "Mock.h"

extern "C" {
#include <stdio.h>
#include <string.h>

#include "URI.h"
#include "StatusLine.h"
#include "MessageBuilder.h"
#include "Messages.h"
#include "ContactHeader.h"
#include "ViaHeader.h"
#include "CallIdHeader.h"
#include "CSeqHeader.h"
#include "Header.h"
#include "TestingMessages.h"
}

TEST_GROUP(TryingMessageBuildTestGroup)
{
};

TEST(TryingMessageBuildTestGroup, TryingMessageStatusLineTest)
{
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);

    MESSAGE *trying = BuildResponse(invite, STATUS_CODE_TRYING);
    struct StatusLine *status = MessageGetStatusLine(trying);
    
    CHECK_TRUE(status != NULL);
    STRCMP_EQUAL(SIP_VERSION, GetSIPVersion(status));
    CHECK_EQUAL(100, GetStatusCode(status));
    STRCMP_EQUAL("Trying", GetReasonPhrase(status));

    DestroyMessage(&trying);
    DestroyMessage(&invite);
}

TEST(TryingMessageBuildTestGroup, TryingMessageFromHeaderTest)
{
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);

    MESSAGE *trying = BuildResponse(invite, STATUS_CODE_TRYING);
    CONTACT_HEADER *inviteFrom = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_FROM, invite);
    CONTACT_HEADER *tryingFrom = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_FROM, trying);

    CHECK_TRUE(ContactHeaderMatched(inviteFrom, tryingFrom));

    DestroyMessage(&trying);
    DestroyMessage(&invite);
}

#define INCOMMING_INVITE_MESSAGE_WITH_TO_TAG (char *)"\
INVITE sip:88001@192.168.10.1:5061;line=987565ffceb68b1 SIP/2.0\r\n\
Via: SIP/2.0/UDP 192.168.10.62:5060;branch=z9hG4bK27dc30b4;rport\r\n\
Max-Forwards: 70\r\n\
From: \"88002\" <sip:88002@192.168.10.62>;tag=as47cc7458\r\n\
To: <sip:88001@192.168.10.1:5061;line=987565ffceb68b1>;tag=ad47cc9876\r\n\
Contact: <sip:88002@192.168.10.62:5060>\r\n\
Call-ID: 00ef25c213336598033d4bf8248deff7@192.168.10.62:5060\r\n\
CSeq: 102 INVITE\r\n\
Content-Length: 284\r\n"

TEST(TryingMessageBuildTestGroup, TryingMessageToWithTagTest)
{
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE_WITH_TO_TAG, invite);
    
    MESSAGE *trying = BuildResponse(invite, STATUS_CODE_TRYING);

    CONTACT_HEADER *inviteTo = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_TO, invite);
    CONTACT_HEADER *tryingTo = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_TO, trying);

    CHECK_TRUE(ContactHeaderMatched(inviteTo, tryingTo));

    DestroyMessage(&trying);
    DestroyMessage(&invite);    
}

TEST(TryingMessageBuildTestGroup, TryingMessageToWithNoTagTest)
{
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    
    MESSAGE *trying = BuildResponse(invite, STATUS_CODE_TRYING);

    CONTACT_HEADER *inviteTo = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_TO, invite);
    CONTACT_HEADER *tryingTo = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_TO, trying);

    URI *inviteToUri = ContactHeaderGetUri(inviteTo);
    URI *tryingToUri = ContactHeaderGetUri(tryingTo);

    CHECK_FALSE(ContactHeaderMatched(inviteTo, tryingTo));
    CHECK_TRUE(UriMatched(inviteToUri, tryingToUri));

    DestroyMessage(&trying);
    DestroyMessage(&invite);    
}

TEST(TryingMessageBuildTestGroup, TryingMessageCallIdTest)
{
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    MESSAGE *trying = BuildResponse(invite, STATUS_CODE_TRYING);

    struct CallIdHeader *inviteCallId = (struct CallIdHeader *)MessageGetHeader(HEADER_NAME_CALLID, invite);
    struct CallIdHeader *tryingCallId = (struct CallIdHeader *)MessageGetHeader(HEADER_NAME_CALLID, trying);
    
    CHECK_TRUE(CallIdHeaderMatched(inviteCallId, tryingCallId));
    
    DestroyMessage(&trying);
    DestroyMessage(&invite);    
}

TEST(TryingMessageBuildTestGroup, TryingMessageCSeqTest)
{
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    MESSAGE *trying = BuildResponse(invite, STATUS_CODE_TRYING);
    
    struct CSeqHeader *inviteCSeq = (struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, invite);
    struct CSeqHeader *tryingCSeq = (struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, trying);

    CHECK_TRUE(CSeqHeadersMatched(inviteCSeq, tryingCSeq));

    DestroyMessage(&trying);
    DestroyMessage(&invite);
}

TEST(TryingMessageBuildTestGroup, TryingMessageViaTest)
{
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    MESSAGE *trying = BuildResponse(invite, STATUS_CODE_TRYING);

    VIA_HEADER *inviteVia = (VIA_HEADER *)MessageGetHeader(HEADER_NAME_VIA, invite);
    VIA_HEADER *tryingVia = (VIA_HEADER *)MessageGetHeader(HEADER_NAME_VIA, trying);
    
    CHECK_TRUE(ViaHeaderMatched(inviteVia, tryingVia));

    DestroyMessage(&trying);
    DestroyMessage(&invite);    
}

TEST(TryingMessageBuildTestGroup, TryingMessageDestPortTest)
{
    MESSAGE *trying = NULL;
    
    MESSAGE *invite = BuildInviteMessage((char *)"88001", (char *)"88002", (char *)"192.168.10.62", 5060);
    trying = BuildResponse(invite, STATUS_CODE_TRYING);

    CHECK_EQUAL(5060,GetMessagePort(trying));

    DestroyMessage(&trying);
    DestroyMessage(&invite);
}

TEST(TryingMessageBuildTestGroup, TryingMessageDesAddrtTest)
{
    MESSAGE *trying = NULL;
    MESSAGE *invite = BuildInviteMessage((char *)"88001", (char *)"88002", (char *)"192.168.10.62", 5060);

    trying = BuildResponse(invite, STATUS_CODE_TRYING);

    STRCMP_EQUAL((char *)"192.168.10.62", GetMessageAddr(trying));

    DestroyMessage(&trying);
    DestroyMessage(&invite);
}
