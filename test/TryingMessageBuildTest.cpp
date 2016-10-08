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
#include "UserAgent.h"
#include "Dialog.h"
#include "TestingMessages.h"
#include "UserAgentManager.h"
#include "AccountManager.h"
#include "DialogManager.h"
#include "Accounts.h"
}

TEST_GROUP(TryingMessageBuildTestGroup)
{
};

TEST(TryingMessageBuildTestGroup, TryingMessageStatusLineTest)
{
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);

    MESSAGE *trying = BuildTryingMessage(NULL, invite);
    struct StatusLine *status = MessageGetStatusLine(trying);
    
    CHECK_TRUE(status != NULL);
    STRCMP_EQUAL(SIP_VERSION, StatusLineGetSipVersion(status));
    CHECK_EQUAL(100, StatusLineGetStatusCode(status));
    STRCMP_EQUAL("Trying", StatusLineGetReasonPhrase(status));

    DestroyMessage(&trying);
    DestroyMessage(&invite);
}

TEST(TryingMessageBuildTestGroup, TryingMessageFromHeaderTest)
{
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);

    MESSAGE *trying = BuildTryingMessage(NULL, invite);
    CONTACT_HEADER *inviteFrom = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_FROM, invite);
    CONTACT_HEADER *tryingFrom = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_FROM, trying);

    CHECK_TRUE(ContactHeaderMatched(inviteFrom, tryingFrom));

    DestroyMessage(&trying);
    DestroyMessage(&invite);
}

TEST(TryingMessageBuildTestGroup, TryingMessageToWithTagTest)
{
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE_WITH_TO_TAG, invite);
    
    MESSAGE *trying = BuildTryingMessage(NULL, invite);

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
    
    MESSAGE *trying = BuildTryingMessage(NULL, invite);

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
    MESSAGE *trying = BuildTryingMessage(NULL, invite);

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
    MESSAGE *trying = BuildTryingMessage(NULL, invite);
    
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
    MESSAGE *trying = BuildTryingMessage(NULL, invite);

    VIA_HEADER *inviteVia = (VIA_HEADER *)MessageGetHeader(HEADER_NAME_VIA, invite);
    VIA_HEADER *tryingVia = (VIA_HEADER *)MessageGetHeader(HEADER_NAME_VIA, trying);
    
    CHECK_TRUE(ViaHeaderMatched(inviteVia, tryingVia));

    DestroyMessage(&trying);
    DestroyMessage(&invite);    
}

TEST(TryingMessageBuildTestGroup, TryingMessageDestPortTest)
{
    MESSAGE *trying = NULL;
    
    AccountInit();
    struct UserAgent *ua = CreateUserAgent(0);
    struct Dialog *dialog = AddDialog(NULL_DIALOG_ID, ua);
    MESSAGE *invite = BuildInviteMessage(dialog, (char *)"88002");

    trying = BuildTryingMessage(dialog, invite);

    CHECK_EQUAL(AccountGetProxyPort(GetAccount(0)),MessageGetDestPort(trying));

    DestroyMessage(&trying);
    DestroyMessage(&invite);

    ClearDialogManager();
    ClearAccountManager();
    DestroyUserAgent(&ua);
}

TEST(TryingMessageBuildTestGroup, TryingMessageDesAddrtTest)
{
    MESSAGE *trying = NULL;
    
    AccountInit();
    struct UserAgent *ua = CreateUserAgent(0);
    struct Dialog *dialog = AddDialog(NULL_DIALOG_ID, ua);
    MESSAGE *invite = BuildInviteMessage(dialog, (char *)"88002");

    trying = BuildTryingMessage(dialog, invite);

    STRCMP_EQUAL(AccountGetProxyAddr(GetAccount(0)),MessageGetDestAddr(trying));

    DestroyMessage(&trying);
    DestroyMessage(&invite);

    ClearDialogManager();
    ClearAccountManager();
    DestroyUserAgent(&ua);
}
