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
#include "WWW_AuthenticationHeader.h"
}

TEST_GROUP(BindingMessageBuildTestGroup)
{
    MESSAGE *m;
    void setup()
    {
        UT_PTR_SET(GenerateBranch, GenerateBranchMock);

        char username[] = "88001";
        char registrar[] = "192.168.10.62";
        int port = 5060;
        m = BuildAddBindingMessage(username, username, registrar, port);
    }

    void teardown()
    {
        DestroyMessage(&m);
    }
};

TEST(BindingMessageBuildTestGroup, NoneMessageTypeTest)
{
    CHECK_EQUAL(MESSAGE_TYPE_REQUEST, MessageType(m));

    MESSAGE *tmpMessage = CreateMessage();
    CHECK_EQUAL(MESSAGE_TYPE_NONE, MessageType(tmpMessage));
    DestroyMessage(&tmpMessage);

}

TEST(BindingMessageBuildTestGroup, AckMessageTypeTest)
{
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    
    MESSAGE *tmpMessage = BuildAckMessage(invite, (char *)"192.168.10.62", 5060);
    CHECK_EQUAL(MESSAGE_TYPE_REQUEST, MessageType(tmpMessage));
    DestroyMessage(&tmpMessage);
    DestroyMessage(&invite);
}

TEST(BindingMessageBuildTestGroup, RequestLineTest)
{

    struct RequestLine *rl = MessageGetRequestLine(m);
    URI *uri = RequestLineGetUri(rl);
    
    STRCMP_EQUAL(URI_SCHEME_SIP, UriGetScheme(uri));
    STRCMP_EQUAL(GetProxy(0), UriGetHost(uri));
    STRCMP_EQUAL("", UriGetUser(uri));
}

TEST(BindingMessageBuildTestGroup, FromHeaderTest)
{
    CONTACT_HEADER *from = (CONTACT_HEADER *) MessageGetHeader(HEADER_NAME_FROM, m);
    STRCMP_EQUAL(HEADER_NAME_FROM, ContactHeaderGetName(from));

    URI *uri = ContactHeaderGetUri(from);
    STRCMP_EQUAL(GetUserName(0), UriGetUser(uri));
    STRCMP_EQUAL(GetProxy(0), UriGetHost(uri));
}

TEST(BindingMessageBuildTestGroup, ToHeaderTest)
{
    CONTACT_HEADER *to = (CONTACT_HEADER *) MessageGetHeader(HEADER_NAME_TO, m);
    STRCMP_EQUAL(HEADER_NAME_TO, ContactHeaderGetName(to));

    URI *uri = ContactHeaderGetUri(to);
    STRCMP_EQUAL(GetUserName(0), UriGetUser(uri));
    STRCMP_EQUAL(GetProxy(0), UriGetHost(uri));
}

TEST(BindingMessageBuildTestGroup, ViaHeaderTest)
{
    VIA_HEADER *via = (VIA_HEADER *) MessageGetHeader(HEADER_NAME_VIA, m);
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
    CONTACT_HEADER *contact = (CONTACT_HEADER *) MessageGetHeader(HEADER_NAME_CONTACT, m);

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
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    MESSAGE *ringing = BuildResponse(invite, STATUS_CODE_RINGING);
    
    struct StatusLine *sl = MessageGetStatusLine(ringing);

    CHECK_EQUAL(180, GetStatusCode(sl));
    STRCMP_EQUAL("Ringing", GetReasonPhrase(sl));

    DestroyMessage(&invite);
    DestroyMessage(&ringing);
}

TEST(BindingMessageBuildTestGroup, OKMessageStatusLineTest)
{
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    MESSAGE *ok = BuildResponse(invite, STATUS_CODE_OK);
    
    struct StatusLine *sl = MessageGetStatusLine(ok);

    CHECK_EQUAL(200, GetStatusCode(sl));
    STRCMP_EQUAL("OK", GetReasonPhrase(sl));

    DestroyMessage(&invite);
    DestroyMessage(&ok);
}

TEST(BindingMessageBuildTestGroup,OKMessageContactHeaderTest)
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

TEST(BindingMessageBuildTestGroup, 301MessageStatueLineTest)
{
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    MESSAGE *moved = BuildResponse(invite, STATUS_CODE_MOVED_PERMANENTLY);
    
    struct StatusLine *sl = MessageGetStatusLine(moved);

    CHECK_EQUAL(301, GetStatusCode(sl));
    STRCMP_EQUAL(REASON_PHRASE_MOVED_PERMANENTLY, GetReasonPhrase(sl));

    DestroyMessage(&invite);
    DestroyMessage(&moved);

}

TEST(BindingMessageBuildTestGroup, BindingsRequestLineTest)
{
    struct RequestLine *rl = MessageGetRequestLine(m);
    STRCMP_EQUAL("REGISTER", RequestLineGetMethodName(rl));
    STRCMP_EQUAL("SIP/2.0", RequestLineGetSipVersion(rl));

    URI *uri = RequestLineGetUri(rl);
    STRCMP_EQUAL(URI_SCHEME_SIP,  UriGetScheme(uri));
    STRCMP_EQUAL(GetProxy(0), UriGetHost(uri));
}

TEST(BindingMessageBuildTestGroup, BindingsToHeaderTest)
{
    CONTACT_HEADER *to = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_TO, m); 
    URI *uri = ContactHeaderGetUri(to);

    STRCMP_EQUAL(GetUserName(0), UriGetUser(uri));
}

TEST(BindingMessageBuildTestGroup, BindingsFromHeaderTest)
{
    CONTACT_HEADER *from = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_FROM, m); 
    URI *uri = ContactHeaderGetUri(from);

    STRCMP_EQUAL(GetUserName(0), UriGetUser(uri));
}

TEST(BindingMessageBuildTestGroup, BindingsContactHeaderTest)
{
    CONTACT_HEADER *contact = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_CONTACT, m); 
    URI *uri = ContactHeaderGetUri(contact);

    STRCMP_EQUAL(GetUserName(0), UriGetUser(uri));
}

TEST(BindingMessageBuildTestGroup, UnbindingMessage)
{
    char username[] = "88001";
    char registrar[] = "192.168.10.62";
    int port = 5060;

    MESSAGE *remove = BuildRemoveBindingMessage(username, username, registrar, port);
    
    struct ExpiresHeader *e = (struct ExpiresHeader *)MessageGetHeader(HEADER_NAME_EXPIRES, remove);
    
    CHECK_EQUAL(0, ExpiresHeaderGetExpires(e));
    DestroyMessage(&remove);
}

TEST(BindingMessageBuildTestGroup, MessageAuthorizationHeaderTest)
{
    MESSAGE *challenge = CreateMessage();
    ParseMessage(UNAUTHORIZED_MESSAGE, challenge);

    MESSAGE *authMessage = BuildAuthorizationMessage(challenge, (char *)"88001", (char *)"88001",
                                                     (char *)"192.168.10.62",
                                                     5060);
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

TEST(BindingMessageBuildTestGroup, AuthorizationMessageToHeaderTest)
{
    MESSAGE *challenge = CreateMessage();
    ParseMessage(UNAUTHORIZED_MESSAGE, challenge);

    MESSAGE *authMessage = BuildAuthorizationMessage(challenge, (char *)"88001", (char *)"88001",
                                                     (char *)"192.168.10.62",
                                                     5060);
    CONTACT_HEADER *toHeader = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_TO, authMessage);
    URI *uri = ContactHeaderGetUri(toHeader);
    STRCMP_EQUAL("88001", UriGetUser(uri));
    
    DestroyMessage(&authMessage);
    DestroyMessage(&challenge);
}

TEST(BindingMessageBuildTestGroup, AuthorizationMessageCSeqHeaderTest)
{
    MESSAGE *challenge = CreateMessage();
    ParseMessage(UNAUTHORIZED_MESSAGE, challenge);

    MESSAGE *authMessage = BuildAuthorizationMessage(challenge, (char *)"88001", (char *)"88001",
                                                     (char *)"192.168.10.62",
                                                     5060);

    CHECK_EQUAL(MessageGetCSeqNumber(challenge) + 1,
                MessageGetCSeqNumber(authMessage));
    
    
    DestroyMessage(&authMessage);
    DestroyMessage(&challenge);
}

TEST(BindingMessageBuildTestGroup, MessageDestAddrTest)
{
    char registrar[] = "192.168.10.62";
    STRCMP_EQUAL(registrar, GetMessageAddr(m));
}


TEST(BindingMessageBuildTestGroup, MessageDestPortTest)
{
    int port = 5060;
    CHECK_EQUAL(port,GetMessagePort(m));
}
