#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include <stdio.h>
#include <strings.h>
#include <string.h>

#include "Parser.h"
#include "Messages.h"
#include "ViaHeader.h"
#include "MaxForwardsHeader.h"
#include "ContactHeader.h"
#include "CallIdHeader.h"
#include "CSeqHeader.h"
#include "ContentLengthHeader.h"
#include "StatusLine.h"
#include "Header.h"
#include "TestingMessages.h"
#include "URI.h"
#include "RequestLine.h"
#include "Sdp.h"
#include "WWW_AuthenticationHeader.h"
#include "Parameter.h"
}

TEST_GROUP(MessageTestGroup)
{
    char messageString[2048];
    void setup()
    {
        strcpy(messageString, "\
REGISTER sip:registrar.munich.de SIP/2.0\r\n\
Via:SIP/2.0/UDP 200.201.202.203:5060;branch=z9hG4bKus19\r\n\
Max-Forwards:70\r\n\
To:\"Werner Heisenberg\"<sip:werner.heisenberg@munich.de>;tag=4321\r\n\
From:\"Werner Heisenberg\"<sip:werner.heisenberg@munich.de>;tag=3431\r\n\
Call-ID:23@200.201.202.203\r\n\
CSeq:1 REGISTER\r\n\
Contact:<sip:werner.heisenberg@200.201.202.203>\r\n\
Content-Length:0\r\n\r\n");
    }
    
    void UriCheck(CONTACT_HEADER *header)
    {
        URI *uri = ContactHeaderGetUri(header);
        STRCMP_EQUAL("sip", UriGetScheme(uri));
        STRCMP_EQUAL("werner.heisenberg", UriGetUser(uri));
        STRCMP_EQUAL("munich.de", UriGetHost(uri));
    }
};

TEST(MessageTestGroup, RegisterRequestLineParseTest)
{
    
    MESSAGE *message = CreateMessage();
    ParseMessage(messageString, message);
    
    struct RequestLine *request = MessageGetRequestLine(message);
    STRCMP_EQUAL("REGISTER", RequestLineGetMethodName(request));
    STRCMP_EQUAL("SIP/2.0", RequestLineGetSipVersion(request));
    
    URI *uri = RequestLineGetUri(request);
    STRCMP_EQUAL("sip", UriGetScheme(uri));
    STRCMP_EQUAL("registrar.munich.de", UriGetHost(uri));
    STRCMP_EQUAL("", UriGetUser(uri));

    DestroyMessage(&message);
}

TEST(MessageTestGroup, ViaHeaderParseTest)
{
    MESSAGE *message = CreateMessage();
    ParseMessage(messageString, message);

    struct Header *header =  MessageGetHeader("Via", message);
    VIA_HEADER *via = (struct ViaHeader*)header;

    STRCMP_EQUAL("Via", ViaHeaderGetName(via));
    STRCMP_EQUAL("SIP/2.0/UDP", ViaHeaderGetTransport(via));
    STRCMP_EQUAL("200.201.202.203", UriGetHost(ViaHeaderGetUri(via)));
    CHECK_EQUAL(5060, UriGetPort(ViaHeaderGetUri(via)));
    STRCMP_EQUAL("z9hG4bKus19", ViaHeaderGetParameter(via, (char *)"branch"));
    DestroyMessage(&message);    
}

TEST(MessageTestGroup, MaxForwardsParseTest)
{
    MESSAGE *message = CreateMessage();
    ParseMessage(messageString, message);

    struct MaxForwardsHeader *m = (struct MaxForwardsHeader *)MessageGetHeader("Max-Forwards", message);
    
    STRCMP_EQUAL("Max-Forwards", MaxForwardsGetName(m));
    CHECK_EQUAL(70, MaxForwardsGetMaxForwards(m));
    
    DestroyMessage(&message);    

}

TEST(MessageTestGroup, ToParseTest)
{
    MESSAGE *message = CreateMessage();
    ParseMessage(messageString, message);

    CONTACT_HEADER *to = (CONTACT_HEADER *) MessageGetHeader("To", message);
    STRCMP_EQUAL("To", ContactHeaderGetName(to));
    STRCMP_EQUAL("Werner Heisenberg", ContactHeaderGetDisplayName(to));
    UriCheck(to);

    DestroyMessage(&message);
}

TEST(MessageTestGroup, FromParseTest)
{
    MESSAGE *message = CreateMessage();
    ParseMessage(messageString, message);

    CONTACT_HEADER *from = (CONTACT_HEADER *) MessageGetHeader("From", message);
    STRCMP_EQUAL("From", ContactHeaderGetName(from));
    STRCMP_EQUAL("Werner Heisenberg", ContactHeaderGetDisplayName(from));
    UriCheck(from);
    STRCMP_EQUAL("3431", ContactHeaderGetParameter(from, HEADER_PARAMETER_NAME_TAG));

    DestroyMessage(&message);

}

TEST(MessageTestGroup, ContactParseTest)
{
    MESSAGE *message = CreateMessage();
    ParseMessage(messageString, message);

    CONTACT_HEADER *contact = (CONTACT_HEADER *) MessageGetHeader("Contact", message);
    STRCMP_EQUAL("Contact", ContactHeaderGetName(contact));

    URI *uri = ContactHeaderGetUri(contact);
    STRCMP_EQUAL("sip", UriGetScheme(uri));
    STRCMP_EQUAL("werner.heisenberg", UriGetUser(uri));
    STRCMP_EQUAL("200.201.202.203", UriGetHost(uri));

    DestroyMessage(&message);
}

TEST(MessageTestGroup, CallIdParseTest)
{
    MESSAGE *message = CreateMessage();
    ParseMessage(messageString, message);

    struct CallIdHeader *id = (struct CallIdHeader *) MessageGetHeader("Call-ID", message);
    STRCMP_EQUAL("Call-ID", CallIdHeaderGetName(id));
    STRCMP_EQUAL("23@200.201.202.203", CallIdHeaderGetId(id));
    DestroyMessage(&message);
} 

TEST(MessageTestGroup, CSeqParseTest)
{
    MESSAGE *message = CreateMessage();
    ParseMessage(messageString, message);

    struct CSeqHeader *c = (struct CSeqHeader *)MessageGetHeader("CSeq", message);
    STRCMP_EQUAL("CSeq", CSeqHeaderGetName(c));
    CHECK_EQUAL(1, CSeqHeaderGetSeq(c));
    STRCMP_EQUAL("REGISTER", CSeqHeaderGetMethod(c));

    DestroyMessage(&message);
}

TEST(MessageTestGroup, ContentLengthParseTest)
{
    MESSAGE *message = CreateMessage();
    ParseMessage(messageString, message);

    struct ContentLengthHeader *c = (struct ContentLengthHeader *) MessageGetHeader("Content-Length", message);
    STRCMP_EQUAL("Content-Length", ContentLengthHeaderGetName(c));
    CHECK_EQUAL(0, ContentLengthHeaderGetLength(c));

    DestroyMessage(&message);
}

TEST(MessageTestGroup, CreateMessageTest)
{
    MESSAGE *message = CreateMessage();

    DestroyMessage(&message);
}

TEST(MessageTestGroup, Message2StringTest)
{
    MESSAGE *message = CreateMessage();
    char result[1024] = {0};

    ParseMessage(messageString, message);
    Message2String(result, message);
    STRCMP_EQUAL(messageString, result);

    DestroyMessage(&message);
}

TEST(MessageTestGroup, Message2StringEndWithCRLFCRLF)
{
    MESSAGE *message = CreateMessage();
    char result[1024] = {0};
    int last;

    ParseMessage(messageString, message);
    Message2String(result, message);
    last = strlen(result) - 1;
    
    CHECK_EQUAL('\r', result[last - 3]);
    CHECK_EQUAL('\n', result[last - 2]);
    CHECK_EQUAL('\r', result[last - 1]);
    CHECK_EQUAL('\n', result[last]);
    
    DestroyMessage(&message);
    
}

TEST(MessageTestGroup, ExtractHeaderNameTest)
{
    char header[] = "Via: SIP/2.0/UDP 200.201.202.203:5060;branch=z9hG4bKus19";
    char name[32] = {0};

    ExtractHeaderName(header, name);
    STRCMP_EQUAL("Via", name);
}

TEST(MessageTestGroup, ResponseMessageParseTest)
{
    MESSAGE *message = CreateMessage();
    char string[] = "\
SIP/2.0 180 Ringing\r\n\
Via:SIP/2.0/UDP server10.biloxi.com;branch=z9hG4bK4b43c2ff8.1\r\n\
To:Bob <sip:bob@biloxi.com>;tag=a6c85cf\r\n\
From:Alice <sip:alice@atlanta.com>;tag=1928301774\r\n\
Call-ID:a84b4c76e66710\r\n\
Contact:<sip:bob@192.0.2.4>\r\n\
CSeq:314159 INVITE\r\n\
Content-Length:0\r\n";

    ParseMessage(string, message);
    StatusLine *s = MessageGetStatusLine(message);
    
    STRCMP_EQUAL("SIP/2.0", StatusLineGetSipVersion(s));
    CHECK_EQUAL(180, StatusLineGetStatusCode(s));
    STRCMP_EQUAL("Ringing", StatusLineGetReasonPhrase(s));

    DestroyMessage(&message);
}

#define BYE_MESSAGE (char *)"\
BYE sip:88001@192.168.10.62:5060 SIP/2.0\r\n\
CSeq: 1 BYE\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5060;branch=z9hG4bK56fb2ea6-fe10-e611-972d-60eb69bfc4e8;rport;send-by=192.168.10.101\r\n\
User-Agent: Ekiga/4.0.1\r\n\
From: <sip:88002@192.168.10.1>;tag=2eb78b83-fe10-e611-972d-60eb69bfc4e8\r\n\
Call-ID: 32ee1a624979941474ca0eab6d2d2b37@192.168.10.62:5060\r\n\
To: \"88001\" <sip:88001@192.168.10.62>;tag=as5c1841ad\r\n\
Contact: \"Martin\" <sip:88002@192.168.10.1:5060>\r\n\
Content-Length: 0\r\n\
Max-Forwards: 70\r\n"

TEST(MessageTestGroup, ByeMessageParseRequestLineMethodTest)
{
    MESSAGE *bye = CreateMessage();
    ParseMessage(BYE_MESSAGE, bye);

    struct RequestLine *rl = MessageGetRequestLine(bye);
    STRCMP_EQUAL(SIP_METHOD_NAME_BYE, RequestLineGetMethodName(rl));

    DestroyMessage(&bye);
}

TEST(MessageTestGroup, ResponseMessage2StringTest)
{
    MESSAGE *message = CreateMessage();
    char string[] = "\
SIP/2.0 180 Ringing\r\n\
Via:SIP/2.0/UDP server10.biloxi.com;branch=z9hG4bK4b43c2ff8.1\r\n\
To:\"Bob\"<sip:bob@biloxi.com>;tag=a6c85cf\r\n\
From:\"Alice\"<sip:alice@atlanta.com>;tag=1928301774\r\n\
Call-ID:a84b4c76e66710\r\n\
Contact:<sip:bob@192.0.2.4>\r\n\
CSeq:314159 INVITE\r\n\
Content-Length:0\r\n\r\n";
    char result[1024] = {0};

    ParseMessage(string, message);
    Message2String(result, message);
    STRCMP_EQUAL(string,result);

    DestroyMessage(&message);
}

TEST(MessageTestGroup, EmptyMessageParseTest)
{
    MESSAGE *message = CreateMessage();
    char string[] = "";
    
    CHECK_EQUAL(-1,ParseMessage(string, message));

    DestroyMessage(&message);
}

TEST(MessageTestGroup, GetContentLengthTest)
{
    MESSAGE *message = CreateMessage();
    char string[] = "\
SIP/2.0 180 Ringing\r\n\
Via:SIP/2.0/UDP server10.biloxi.com;branch=z9hG4bK4b43c2ff8.1\r\n\
To:\"Bob\"<sip:bob@biloxi.com>;tag=a6c85cf\r\n\
From:\"Alice\"<sip:alice@atlanta.com>;tag=1928301774\r\n\
Call-ID:a84b4c76e66710\r\n\
Contact:<sip:bob@192.0.2.4>\r\n\
CSeq:314159 INVITE\r\n\
Content-Length:100\r\n";
    ParseMessage(string, message);
    CHECK_EQUAL(100, MessageGetContentLength(message));

    DestroyMessage(&message);

}

TEST(MessageTestGroup, GetContentLengthMessageWithNoContentLengthHeader)
{
    MESSAGE *message = CreateMessage();
    char string[] = "\
SIP/2.0 180 Ringing\r\n\
Via:SIP/2.0/UDP server10.biloxi.com;branch=z9hG4bK4b43c2ff8.1\r\n\
To:\"Bob\"<sip:bob@biloxi.com>;tag=a6c85cf\r\n\
From:\"Alice\"<sip:alice@atlanta.com>;tag=1928301774\r\n        \
Call-ID:a84b4c76e66710\r\n                              \
Contact:<sip:bob@192.0.2.4>\r\n                 \
CSeq:314159 INVITE\r\n";
    ParseMessage(string, message);
    CHECK_EQUAL(0, MessageGetContentLength(message));

    DestroyMessage(&message);


}

TEST(MessageTestGroup, SetContentLengthTest)
{
    MESSAGE *message = CreateMessage();
    ParseMessage(messageString, message);

    MessageSetContentLength(message, 1024);
    CHECK_EQUAL(1024, ContentLengthHeaderGetLength(
                    (struct ContentLengthHeader *)MessageGetHeader(HEADER_NAME_CONTENT_LENGTH, message)));

    DestroyMessage(&message);
}

TEST(MessageTestGroup, GetCallIdTest)
{
    MESSAGE *message = CreateMessage();
    ParseMessage(messageString, message);

    STRCMP_EQUAL("23@200.201.202.203", MessageGetCallId(message));
    DestroyMessage(&message);
}

TEST(MessageTestGroup, GetLocalTageTest)
{
    MESSAGE *message = CreateMessage();
    ParseMessage(messageString, message);

    STRCMP_EQUAL("3431", MessageGetFromTag(message));
    DestroyMessage(&message);
}

TEST(MessageTestGroup, GetRemoteTageTest)
{
    MESSAGE *message = CreateMessage();
    ParseMessage(messageString, message);

    STRCMP_EQUAL("4321", MessageGetToTag(message));
    DestroyMessage(&message);
}

TEST(MessageTestGroup, GetExpiresTest)
{
    MESSAGE *message = CreateMessage();
    ParseMessage(ADD_BINDING_OK_MESSAGE, message);

    CHECK_EQUAL(3600, MessageGetExpires(message));
    DestroyMessage(&message);
}

TEST(MessageTestGroup, GetBranchTest)
{
    MESSAGE *localMessage = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, localMessage);

    STRCMP_EQUAL("z9hG4bK27dc30b4",MessageGetViaBranch(localMessage));
   
    DestroyMessage(&localMessage);
}

TEST(MessageTestGroup, SetBranchTest)
{
    char newBranch[] = "0123456789";
    MESSAGE *localMessage = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, localMessage);
    MessageSetViaBranch(localMessage, newBranch);

    STRCMP_EQUAL(newBranch, MessageGetViaBranch(localMessage));
   
    DestroyMessage(&localMessage);

}

TEST(MessageTestGroup, GetCSeqMethodTest)
{
    MESSAGE *localMessage = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, localMessage);

    STRCMP_EQUAL("INVITE",MessageGetCSeqMethod(localMessage));
   
    DestroyMessage(&localMessage);
}

#define MESSAGE_WITH_WRONG_HEADER "\
ACK sip:3401@10.6.2.10:5060 SIP/2.0\r\n\
Via: SIP/2.0/UDP  10.6.3.1:5060;branch=z9hG4bKB1C57\r\n\
From: <sip:4105553501@10.6.3.1>;tag=105741C-1D5E\r\n\
ASDFASDFa\r\n\
:\r\n\
::::\r\n\
To: <sip:3401@10.6.2.10>;tag=16777231\r\n\
Date: Fri, 06 Jan 2006 5:35:13 GMT\r\n\
Call-ID: E937365B-2C0C11D6-802FA93D-4772A3BB@10.6.3.1\r\n\
Max-Forwards: 70\r\n\
CSeq: 101 ACK\r\n\
Content-Length: 0\r\n"

TEST(MessageTestGroup, ParseMessageWithWrongHeaderTest)
{
    MESSAGE *localMessage = CreateMessage();
    ParseMessage(MESSAGE_WITH_WRONG_HEADER, localMessage);

    DestroyMessage(&localMessage);
}

#define MESSAGE_WITH_LONG_HEADER_NAME "\
ACK sip:3401@10.6.2.10:5060 SIP/2.0\r\n\
Via: SIP/2.0/UDP  10.6.3.1:5060;branch=z9hG4bKB1C57\r\n\
From: <sip:4105553501@10.6.3.1>;tag=105741C-1D5E\r\n\
ASDFASDFaasdfasfsadfasdfsadfsadfsadfasdfasdfsadfasdfsadfasdfasfsafsafasfsafsaf:\r\n\
To: <sip:3401@10.6.2.10>;tag=16777231\r\n\
Date: Fri, 06 Jan 2006 5:35:13 GMT\r\n\
Call-ID: E937365B-2C0C11D6-802FA93D-4772A3BB@10.6.3.1\r\n\
Max-Forwards: 70\r\n\
CSeq: 101 ACK\r\n\
Content-Length: 0\r\n"

TEST(MessageTestGroup, ParseMessageWithLongHeaderNameTest)
{
    MESSAGE *localMessage = CreateMessage();
    ParseMessage(MESSAGE_WITH_LONG_HEADER_NAME, localMessage);

    DestroyMessage(&localMessage);
}
 
int ParseSdpMock(char *content, unsigned int length)
{
    char con[8] = {0};
    
    mock().actualCall("ParseSdp").withParameter("length", length);
    strncpy(con, content, 3);

    STRCMP_EQUAL("v=0", con);
    CHECK_EQUAL(435, strlen(content));

    return 0;
}
#define MESSAGE_WITH_CONTENT "\
INVITE sip:88002@192.168.10.62 SIP/2.0\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5061;rport;branch=z9hG4bK826287637\r\n\
From: <sip:88001@192.168.10.62>;tag=701705428\r\n\
To: <sip:88002@192.168.10.62>\r\n\
Call-ID: 955839666\r\n\
CSeq: 20 INVITE\r\n\
Contact: <sip:martin@(null)>\r\n\
Content-Type: application/sdp\r\n\
Allow: INVITE, ACK, CANCEL, OPTIONS, BYE, REFER, NOTIFY, MESSAGE, SUBSCRIBE, INFO\r\n\
Max-Forwards: 70\r\n\
User-Agent: Linphone/3.6.1 (eXosip2/4.0.0)\r\n\
Subject: Phone call\r\n\
Content-Length:   435\r\n\
\r\n\
v=0\r\n\
o=88001 686 894 IN IP4 192.168.10.1\r\n\
s=Talk\r\n\
c=IN IP4 192.168.10.1\r\n\
t=0 0\r\n\
m=audio 7078 RTP/AVP 124 111 110 0 8 101\r\n\
a=rtpmap:124 opus/48000\r\n\
a=fmtp:124 useinbandfec=1; usedtx=1\r\n\
a=rtpmap:111 speex/16000\r\n\
a=fmtp:111 vbr=on\r\n\
a=rtpmap:110 speex/8000\r\n\
a=fmtp:110 vbr=no\r\n\
a=rtpmap:101 telephone-event/8000\r\n\
a=fmtp:101 0-11\r\n\
m=video 9078 RTP/AVP 103 99\r\n\
a=rtpmap:103 VP8/90000\r\n\
a=rtpmap:99 MP4V-ES/90000\r\n\
a=fmtp:99 profile-level-id=3\r\n"

TEST(MessageTestGroup, ParseMessageWithContentTest)
{
    MESSAGE *localMessage = CreateMessage();

    UT_PTR_SET(ParseSdp, ParseSdpMock);
    mock().expectOneCall("ParseSdp").withParameter("length", 435);
    
    ParseMessage(MESSAGE_WITH_CONTENT, localMessage);

    mock().checkExpectations();
    mock().clear();
    DestroyMessage(&localMessage);
}

TEST(MessageTestGroup, GetInviteRequestMethodTest)
{
    MESSAGE *localMessage = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, localMessage);

    CHECK_EQUAL(SIP_METHOD_INVITE, MessageGetMethod(localMessage));
    DestroyMessage(&localMessage);
}

TEST(MessageTestGroup, GetRegisterRequestMethodTest)
{
    MESSAGE *localMessage = CreateMessage();
    ParseMessage(messageString, localMessage);

    CHECK_EQUAL(SIP_METHOD_REGISTER, MessageGetMethod(localMessage));
    DestroyMessage(&localMessage);
}

TEST(MessageTestGroup, GetNonRequestMethodTest)
{
    char string[] = "\
SIP/2.0 180 Ringing\r\n\
Via:SIP/2.0/UDP server10.biloxi.com;branch=z9hG4bK4b43c2ff8.1\r\n\
To:Bob <sip:bob@biloxi.com>;tag=a6c85cf\r\n\
From:Alice <sip:alice@atlanta.com>;tag=1928301774\r\n\
Call-ID:a84b4c76e66710\r\n\
Contact:<sip:bob@192.0.2.4>\r\n\
CSeq:314159 INVITE\r\n\
Content-Length:0\r\n";
    MESSAGE *localMessage = CreateMessage();
    ParseMessage(string, localMessage);

    CHECK_EQUAL(SIP_METHOD_NONE, MessageGetMethod(localMessage));
    DestroyMessage(&localMessage);
}

TEST(MessageTestGroup, AuthHeaderParseTest)
{
    MESSAGE *message = CreateMessage();

    ParseMessage(UNAUTHORIZED_MESSAGE, message);
    struct AuthHeader *auth = (struct AuthHeader *)MessageGetHeader(HEADER_NAME_WWW_AUTHENTICATE, message);
    struct Parameters *ps = AuthHeaderGetParameters(auth);

    STRCMP_EQUAL("MD5", GetParameter(ps, (char *)"algorithm"));
    STRCMP_EQUAL("\"asterisk\"", GetParameter(ps, (char *)"realm"));
    STRCMP_EQUAL("\"1cd2586e\"", GetParameter(ps, (char *)"nonce"));
    
    DestroyMessage(&message);
}

#define  BINDING_MESSAGE_WITH_AUTHORIZATION (char *)"\
REGISTER sip:192.168.10.62 SIP/2.0\r\n\
Via:SIP/2.0/UDP 192.168.10.1:5064;rport;branch=z9hG4bK654649461\r\n\
From:<sip:88004@192.168.10.62>;tag=930859246\r\n\
To:<sip:88004@192.168.10.62>\r\n\
Call-ID:385886699\r\n\
CSeq:22 REGISTER\r\n\
Contact:<sip:88004@192.168.10.1:5064;line=72cc7a7ac1d6a80>\r\n\
Authorization:Digest username=\"88004\",realm=\"asterisk\",nonce=\"40062bed\",uri=\"sip:192.168.10.62\",response=\"ff880a705d5848ea0b81bdfbce0ea782\",algorithm=MD5\r\n\
Max-Forwards:70\r\n\
Expires:3600\r\n\
Content-Length:0\r\n\r\n"

TEST(MessageTestGroup, AuthorizationHeader2StringTest)
{
    MESSAGE *message = CreateMessage();
    char result[MAX_MESSAGE_LENGTH] = {0};

    ParseMessage(BINDING_MESSAGE_WITH_AUTHORIZATION, message);

    Message2String(result, message);
    STRCMP_EQUAL(BINDING_MESSAGE_WITH_AUTHORIZATION, result);
    
    DestroyMessage(&message);    
}
