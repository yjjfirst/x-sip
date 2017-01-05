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
#include "Accounts.h"
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
        dialog = AddDialog(NULL_DIALOG_ID, ua);
        inviteMessage = DialogBuildInvite(dialog , (char *)"88002");
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

TEST(InviteMessageBuildTestGroup, RingingMessageStatusLineTest)
{
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    MESSAGE *ringing = BuildResponse(invite, STATUS_CODE_RINGING);
    
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
    MESSAGE *ok = BuildResponse(invite, STATUS_CODE_OK);    
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
    MESSAGE *ok = BuildResponse(invite, STATUS_CODE_OK);
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
    Message *moved = BuildResponse(invite, STATUS_CODE_MOVED_PERMANENTLY);
    
    struct StatusLine *sl = MessageGetStatusLine(moved);

    CHECK_EQUAL(301, StatusLineGetStatusCode(sl));
    STRCMP_EQUAL(REASON_PHRASE_MOVED_PERMANENTLY, StatusLineGetReasonPhrase(sl));

    DestroyMessage(&invite);
    DestroyMessage(&moved);

}

TEST(InviteMessageBuildTestGroup, 487MessageStatueLineTest)
{
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    Message *moved = BuildResponse(invite, STATUS_CODE_REQUEST_TERMINATED);
    
    struct StatusLine *sl = MessageGetStatusLine(moved);

    CHECK_EQUAL(487, StatusLineGetStatusCode(sl));
    STRCMP_EQUAL(REASON_PHRASE_REQUEST_TERMINATED, StatusLineGetReasonPhrase(sl));

    DestroyMessage(&invite);
    DestroyMessage(&moved);

}

IGNORE_TEST(InviteMessageBuildTestGroup, ByeMessageToHeaderTest)
{
    MESSAGE *bye = BuildByeMessage();
    CONTACT_HEADER *to = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_TO, bye);
    URI *uri = ContactHeaderGetUri(to);
    URI *remoteUri = DialogGetRemoteUri(dialog);

    CHECK_TRUE(UriMatched(uri, remoteUri));
    STRCMP_EQUAL(DialogGetRemoteTag(dialog), ContactHeaderGetParameter(to, HEADER_PARAMETER_NAME_TAG));
    
    DestroyMessage(&bye);
}


TEST(InviteMessageBuildTestGroup, ByeMessageRequestLineTest)
{
    MESSAGE *bye = BuildByeMessage();
    struct RequestLine *rl = MessageGetRequestLine(bye);
    URI *uri = RequestLineGetUri(rl);
    URI *remoteUri = DialogGetRemoteUri(dialog);
    
    STRCMP_EQUAL(SIP_METHOD_NAME_BYE, RequestLineGetMethodName(rl));
    STRCMP_EQUAL(SIP_VERSION, RequestLineGetSipVersion(rl));
    CHECK_TRUE(UriMatched(remoteUri, uri));

    DestroyMessage(&bye);
}

TEST(InviteMessageBuildTestGroup, OkMessageDestAddrTest)
{
    MESSAGE *ok = BuildResponse(inviteMessage, STATUS_CODE_OK);
    STRCMP_EQUAL(AccountGetProxyAddr(GetAccount(0)),GetMessageAddr(ok));    

    DestroyMessage(&ok);
}


TEST(InviteMessageBuildTestGroup, OkMessageDestPortTest)
{
    MESSAGE *ok = BuildResponse(inviteMessage, STATUS_CODE_OK);
    CHECK_EQUAL(AccountGetProxyPort(GetAccount(0)),GetMessagePort(ok));    

    DestroyMessage(&ok);
}
