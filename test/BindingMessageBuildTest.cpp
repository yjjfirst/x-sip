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

TEST_GROUP(BindingMessageBuildTestGroup)
{
    struct UserAgent *ua;
    struct Message *m;
    struct Dialog *dialog;
    void setup()
    {
        UT_PTR_SET(GenerateBranch, GenerateBranchMock);
        
        AccountInit();
        ua = CreateUserAgent(0);
        dialog = AddNewDialog(NULL_DIALOG_ID, ua);

        m = BuildAddBindingMessage(dialog);
    }

    void teardown()
    {
        ClearAccountManager();
        DestroyMessage(&m);
        DestroyUserAgent(&ua);
    }
};

TEST(BindingMessageBuildTestGroup, MessageTypeTest)
{
    CHECK_EQUAL(MESSAGE_TYPE_REQUEST, MessageGetType(m));

    struct Message *tmpMessage = CreateMessage();
    CHECK_EQUAL(MESSAGE_TYPE_NONE, MessageGetType(tmpMessage));
    DestroyMessage(&tmpMessage);

    tmpMessage = BuildAckMessage(dialog);
    CHECK_EQUAL(MESSAGE_TYPE_REQUEST, MessageGetType(tmpMessage));
    DestroyMessage(&tmpMessage);
}

TEST(BindingMessageBuildTestGroup, RequestLineTest)
{

    struct RequestLine *rl = MessageGetRequestLine(m);
    struct URI *uri = RequestLineGetUri(rl);
    
    STRCMP_EQUAL(URI_SCHEME_SIP, UriGetScheme(uri));
    STRCMP_EQUAL(GetProxy(0), UriGetHost(uri));
    STRCMP_EQUAL("", UriGetUser(uri));
}

TEST(BindingMessageBuildTestGroup, FromHeaderTest)
{
    struct ContactHeader *from = (struct ContactHeader *) MessageGetHeader(HEADER_NAME_FROM, m);
    STRCMP_EQUAL(HEADER_NAME_FROM, ContactHeaderGetName(from));

    struct URI *uri = ContactHeaderGetUri(from);
    STRCMP_EQUAL(GetUserName(0), UriGetUser(uri));
    STRCMP_EQUAL(GetProxy(0), UriGetHost(uri));
}

TEST(BindingMessageBuildTestGroup, ToHeaderTest)
{
    struct ContactHeader *to = (struct ContactHeader *) MessageGetHeader(HEADER_NAME_TO, m);
    STRCMP_EQUAL(HEADER_NAME_TO, ContactHeaderGetName(to));

    struct URI *uri = ContactHeaderGetUri(to);
    STRCMP_EQUAL(GetUserName(0), UriGetUser(uri));
    STRCMP_EQUAL(GetProxy(0), UriGetHost(uri));
}

TEST(BindingMessageBuildTestGroup, ViaHeaderTest)
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

TEST(BindingMessageBuildTestGroup, ContactHeaderTest)
{
    struct ContactHeader *contact = (struct ContactHeader *) MessageGetHeader(HEADER_NAME_CONTACT, m);

    STRCMP_EQUAL(HEADER_NAME_CONTACT, ContactHeaderGetName(contact));
}

TEST(BindingMessageBuildTestGroup, MaxForwardHeaderTest)
{
    struct MaxForwardsHeader *mf = (struct MaxForwardsHeader *) MessageGetHeader(HEADER_NAME_MAX_FORWARDS, m);
    
    STRCMP_EQUAL(HEADER_NAME_MAX_FORWARDS, MaxForwardsGetName(mf));
}


TEST(BindingMessageBuildTestGroup, CallIdHeaderTest)
{
    struct CallIdHeader *id = (struct CallIdHeader *) MessageGetHeader(HEADER_NAME_CALLID, m);

    STRCMP_EQUAL(HEADER_NAME_CALLID, CallIdHeaderGetName(id));
}

TEST(BindingMessageBuildTestGroup, CSeqHeaerTest)
{
    struct CSeqHeader *cseq = (struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, m);
    
    STRCMP_EQUAL(HEADER_NAME_CSEQ, CSeqHeaderGetName(cseq));
}

TEST(BindingMessageBuildTestGroup, ExpiresHeaderTest)
{
    struct ExpiresHeader *e = (struct ExpiresHeader *) MessageGetHeader(HEADER_NAME_EXPIRES, m);
    
    STRCMP_EQUAL(HEADER_NAME_EXPIRES, ExpiresHeaderGetName(e));
}

TEST(BindingMessageBuildTestGroup, ContentLengthTest)
{
    struct ContentLengthHeader *c = (struct ContentLengthHeader *)MessageGetHeader(HEADER_NAME_CONTENT_LENGTH, m);

    STRCMP_EQUAL(HEADER_NAME_CONTENT_LENGTH, ContentLengthHeaderGetName(c));
}

TEST(BindingMessageBuildTestGroup, RingingMessageStatusLineTest)
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

TEST(BindingMessageBuildTestGroup, OKMessageStatusLineTest)
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

TEST(BindingMessageBuildTestGroup,OKMessageContactHeaderTest)
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

TEST(BindingMessageBuildTestGroup, 301MessageStatueLineTest)
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

struct URI *dialog_remote_target;
struct URI *DialogGetRemoteTargetMock(struct Dialog *dialog)
{
    if (dialog_remote_target == NULL) {
        dialog_remote_target = CreateEmptyUri();
        ParseUri((char *)"sip:8800222@192.168.10.62", &dialog_remote_target);
    }
    return dialog_remote_target;
}

TEST(BindingMessageBuildTestGroup, ByeMessageRequestLineTest)
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

TEST(BindingMessageBuildTestGroup, BindingsRequestLineTest)
{
    struct RequestLine *rl = MessageGetRequestLine(m);
    STRCMP_EQUAL("REGISTER", RequestLineGetMethodName(rl));
    STRCMP_EQUAL("SIP/2.0", RequestLineGetSipVersion(rl));

    struct URI *uri = RequestLineGetUri(rl);
    STRCMP_EQUAL(URI_SCHEME_SIP,  UriGetScheme(uri));
    STRCMP_EQUAL(GetProxy(0), UriGetHost(uri));
}

TEST(BindingMessageBuildTestGroup, BindingsToHeaderTest)
{
    struct ContactHeader *to = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_TO, m); 
    struct URI *uri = ContactHeaderGetUri(to);

    STRCMP_EQUAL(GetUserName(0), UriGetUser(uri));
}

TEST(BindingMessageBuildTestGroup, BindingsFromHeaderTest)
{
    struct ContactHeader *from = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_FROM, m); 
    struct URI *uri = ContactHeaderGetUri(from);

    STRCMP_EQUAL(GetUserName(0), UriGetUser(uri));
}

TEST(BindingMessageBuildTestGroup, BindingsContactHeaderTest)
{
    struct ContactHeader *contact = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_CONTACT, m); 
    struct URI *uri = ContactHeaderGetUri(contact);

    STRCMP_EQUAL(GetUserName(0), UriGetUser(uri));
}

TEST(BindingMessageBuildTestGroup, UnbindingMessage)
{
    struct Message *remove = BuildRemoveBindingMessage(dialog);
    struct ExpiresHeader *e = (struct ExpiresHeader *)MessageGetHeader(HEADER_NAME_EXPIRES, remove);
    
    CHECK_EQUAL(0, ExpiresHeaderGetExpires(e));
    DestroyMessage(&remove);
}

TEST(BindingMessageBuildTestGroup, AuthorizationMessage)
{
    struct Message *challenge = CreateMessage();
    ParseMessage(UNAUTHORIZED_MESSAGE, challenge);

    struct Message *authMessage = BuildAuthorizationMessage(dialog,challenge);
    struct AuthHeader *authHeader = (struct AuthHeader *)MessageGetHeader(HEADER_NAME_AUTHORIZATION, authMessage);
    
    CHECK_FALSE(authHeader == NULL);
    CHECK_EQUAL(DIGEST, AuthHeaderGetScheme(authHeader));
    STRCMP_EQUAL(ALGORITHM_MD5, AuthHeaderGetParameter(authHeader, AUTH_HEADER_ALGORITHM));
    STRCMP_EQUAL("\"88001\"", AuthHeaderGetParameter(authHeader, AUTH_HEADER_USER_NAME));
    STRCMP_EQUAL("\"sip:192.168.10.62\"", AuthHeaderGetParameter(authHeader, AUTH_HEADER_URI));
    STRCMP_EQUAL("\"asterisk\"", AuthHeaderGetParameter(authHeader, AUTH_HEADER_REALM));
    STRCMP_EQUAL("\"1cd2586e\"", AuthHeaderGetParameter(authHeader, AUTH_HEADER_NONCE));
    STRCMP_EQUAL("\"722b5f3120526c9bce5ee1e95a085c1c\"", AuthHeaderGetParameter(authHeader, AUTH_HEADER_RESPONSE));

    DestroyMessage(&authMessage);
    DestroyMessage(&challenge);
}