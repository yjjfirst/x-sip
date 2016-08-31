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
    
    void UriCheck(struct ContactHeader *header)
    {
        struct URI *uri = ContactHeaderGetUri(header);
        STRCMP_EQUAL("sip", UriGetScheme(uri));
        STRCMP_EQUAL("werner.heisenberg", UriGetUser(uri));
        STRCMP_EQUAL("munich.de", UriGetHost(uri));
    }
};

TEST(MessageTestGroup, RegisterRequestLineParseTest)
{
    
    struct Message *message = CreateMessage();
    ParseMessage(messageString, message);
    
    struct RequestLine *request = MessageGetRequestLine(message);
    STRCMP_EQUAL("REGISTER", RequestLineGetMethodName(request));
    STRCMP_EQUAL("SIP/2.0", RequestLineGetSipVersion(request));
    
    struct URI *uri = RequestLineGetUri(request);
    STRCMP_EQUAL("sip", UriGetScheme(uri));
    STRCMP_EQUAL("registrar.munich.de", UriGetHost(uri));
    STRCMP_EQUAL("", UriGetUser(uri));

    DestroyMessage(&message);
}

TEST(MessageTestGroup, ViaHeaderParseTest)
{
    struct Message *message = CreateMessage();
    ParseMessage(messageString, message);

    struct Header *header =  MessageGetHeader("Via", message);
    struct ViaHeader *via = (struct ViaHeader*)header;

    STRCMP_EQUAL("Via", ViaHeaderGetName(via));
    STRCMP_EQUAL("SIP/2.0/UDP", ViaHeaderGetTransport(via));
    STRCMP_EQUAL("200.201.202.203", UriGetHost(ViaHeaderGetUri(via)));
    CHECK_EQUAL(5060, UriGetPort(ViaHeaderGetUri(via)));
    STRCMP_EQUAL("z9hG4bKus19", ViaHeaderGetParameter(via, (char *)"branch"));
    DestroyMessage(&message);    
}

TEST(MessageTestGroup, MaxForwardsParseTest)
{
    struct Message *message = CreateMessage();
    ParseMessage(messageString, message);

    struct MaxForwardsHeader *m = (struct MaxForwardsHeader *)MessageGetHeader("Max-Forwards", message);
    
    STRCMP_EQUAL("Max-Forwards", MaxForwardsGetName(m));
    CHECK_EQUAL(70, MaxForwardsGetMaxForwards(m));
    
    DestroyMessage(&message);    

}

TEST(MessageTestGroup, ToParseTest)
{
    struct Message *message = CreateMessage();
    ParseMessage(messageString, message);

    struct ContactHeader *to = (struct ContactHeader *) MessageGetHeader("To", message);
    STRCMP_EQUAL("To", ContactHeaderGetName(to));
    STRCMP_EQUAL("Werner Heisenberg", ContactHeaderGetDisplayName(to));
    UriCheck(to);

    DestroyMessage(&message);
}

TEST(MessageTestGroup, FromParseTest)
{
    struct Message *message = CreateMessage();
    ParseMessage(messageString, message);

    struct ContactHeader *from = (struct ContactHeader *) MessageGetHeader("From", message);
    STRCMP_EQUAL("From", ContactHeaderGetName(from));
    STRCMP_EQUAL("Werner Heisenberg", ContactHeaderGetDisplayName(from));
    UriCheck(from);
    STRCMP_EQUAL("3431", ContactHeaderGetParameter(from, HEADER_PARAMETER_NAME_TAG));

    DestroyMessage(&message);

}

TEST(MessageTestGroup, ContactParseTest)
{
    struct Message *message = CreateMessage();
    ParseMessage(messageString, message);

    struct ContactHeader *contact = (struct ContactHeader *) MessageGetHeader("Contact", message);
    STRCMP_EQUAL("Contact", ContactHeaderGetName(contact));

    struct URI *uri = ContactHeaderGetUri(contact);
    STRCMP_EQUAL("sip", UriGetScheme(uri));
    STRCMP_EQUAL("werner.heisenberg", UriGetUser(uri));
    STRCMP_EQUAL("200.201.202.203", UriGetHost(uri));

    DestroyMessage(&message);
}

TEST(MessageTestGroup, CallIdParseTest)
{
    struct Message *message = CreateMessage();
    ParseMessage(messageString, message);

    struct CallIdHeader *id = (struct CallIdHeader *) MessageGetHeader("Call-ID", message);
    STRCMP_EQUAL("Call-ID", CallIdHeaderGetName(id));
    STRCMP_EQUAL("23@200.201.202.203", CallIdHeaderGetId(id));
    DestroyMessage(&message);
} 

TEST(MessageTestGroup, CSeqParseTest)
{
    struct Message *message = CreateMessage();
    ParseMessage(messageString, message);

    struct CSeqHeader *c = (struct CSeqHeader *)MessageGetHeader("CSeq", message);
    STRCMP_EQUAL("CSeq", CSeqHeaderGetName(c));
    CHECK_EQUAL(1, CSeqHeaderGetSeq(c));
    STRCMP_EQUAL("REGISTER", CSeqHeaderGetMethod(c));

    DestroyMessage(&message);
}

TEST(MessageTestGroup, ContentLengthParseTest)
{
    struct Message *message = CreateMessage();
    ParseMessage(messageString, message);

    struct ContentLengthHeader *c = (struct ContentLengthHeader *) MessageGetHeader("Content-Length", message);
    STRCMP_EQUAL("Content-Length", ContentLengthHeaderGetName(c));
    CHECK_EQUAL(0, ContentLengthHeaderGetLength(c));

    DestroyMessage(&message);
}

TEST(MessageTestGroup, CreateMessageTest)
{
    struct Message *message = CreateMessage();

    DestroyMessage(&message);
}

TEST(MessageTestGroup, Message2StringTest)
{
    struct Message *message = CreateMessage();
    char result[1024] = {0};

    ParseMessage(messageString, message);
    Message2String(result, message);
    STRCMP_EQUAL(messageString, result);

    DestroyMessage(&message);
}

TEST(MessageTestGroup, Message2StringEndWithCRLFCRLF)
{
    struct Message *message = CreateMessage();
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
    struct Message *message = CreateMessage();
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

TEST(MessageTestGroup, ByeMessageParseRequestLineMethodTest)
{
    struct Message *bye = CreateMessage();
    ParseMessage(BYE_MESSAGE, bye);

    struct RequestLine *rl = MessageGetRequestLine(bye);
    STRCMP_EQUAL(SIP_METHOD_NAME_BYE, RequestLineGetMethodName(rl));

    DestroyMessage(&bye);
}

TEST(MessageTestGroup, ResponseMessage2StringTest)
{
    struct Message *message = CreateMessage();
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
    struct Message *message = CreateMessage();
    char string[] = "";
    
    CHECK_EQUAL(-1,ParseMessage(string, message));

    DestroyMessage(&message);
}

TEST(MessageTestGroup, GetContentLengthTest)
{
    struct Message *message = CreateMessage();
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
    struct Message *message = CreateMessage();
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
    struct Message *message = CreateMessage();
    ParseMessage(messageString, message);

    MessageSetContentLength(message, 1024);
    CHECK_EQUAL(1024, ContentLengthHeaderGetLength(
                    (struct ContentLengthHeader *)MessageGetHeader(HEADER_NAME_CONTENT_LENGTH, message)));

    DestroyMessage(&message);
}

TEST(MessageTestGroup, GetCallIdTest)
{
    struct Message *message = CreateMessage();
    ParseMessage(messageString, message);

    STRCMP_EQUAL("23@200.201.202.203", MessageGetCallId(message));
    DestroyMessage(&message);
}

TEST(MessageTestGroup, GetLocalTageTest)
{
    struct Message *message = CreateMessage();
    ParseMessage(messageString, message);

    STRCMP_EQUAL("3431", MessageGetFromTag(message));
    DestroyMessage(&message);
}

TEST(MessageTestGroup, GetRemoteTageTest)
{
    struct Message *message = CreateMessage();
    ParseMessage(messageString, message);

    STRCMP_EQUAL("4321", MessageGetToTag(message));
    DestroyMessage(&message);
}

TEST(MessageTestGroup, GetExpiresTest)
{
    struct Message *message = CreateMessage();
    ParseMessage(ADD_BINDING_OK_MESSAGE, message);

    CHECK_EQUAL(3600, MessageGetExpires(message));
    DestroyMessage(&message);
}

TEST(MessageTestGroup, GetBranchTest)
{
    struct Message *localMessage = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, localMessage);

    STRCMP_EQUAL("z9hG4bK27dc30b4",MessageGetViaBranch(localMessage));
   
    DestroyMessage(&localMessage);
}

TEST(MessageTestGroup, SetBranchTest)
{
    char newBranch[] = "0123456789";
    struct Message *localMessage = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, localMessage);
    MessageSetViaBranch(localMessage, newBranch);

    STRCMP_EQUAL(newBranch, MessageGetViaBranch(localMessage));
   
    DestroyMessage(&localMessage);

}

TEST(MessageTestGroup, GetCSeqMethodTest)
{
    struct Message *localMessage = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, localMessage);

    STRCMP_EQUAL("INVITE",MessageGetCSeqMethod(localMessage));
   
    DestroyMessage(&localMessage);
}

TEST(MessageTestGroup, ParseMessageWithWrongHeaderTest)
{
    struct Message *localMessage = CreateMessage();
    ParseMessage(MESSAGE_WITH_WRONG_HEADER, localMessage);

    DestroyMessage(&localMessage);
}

TEST(MessageTestGroup, ParseMessageWithLongHeaderNameTest)
{
    struct Message *localMessage = CreateMessage();
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

TEST(MessageTestGroup, ParseMessageWithContentTest)
{
    struct Message *localMessage = CreateMessage();

    UT_PTR_SET(ParseSdp, ParseSdpMock);
    mock().expectOneCall("ParseSdp").withParameter("length", 435);
    
    ParseMessage(MESSAGE_WITH_CONTENT, localMessage);

    mock().checkExpectations();
    mock().clear();
    DestroyMessage(&localMessage);
}

TEST(MessageTestGroup, GetInviteRequestMethodTest)
{
    struct Message *localMessage = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, localMessage);

    CHECK_EQUAL(SIP_METHOD_INVITE, MessageGetMethod(localMessage));
    DestroyMessage(&localMessage);
}

TEST(MessageTestGroup, GetRegisterRequestMethodTest)
{
    struct Message *localMessage = CreateMessage();
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
    struct Message *localMessage = CreateMessage();
    ParseMessage(string, localMessage);

    CHECK_EQUAL(SIP_METHOD_NONE, MessageGetMethod(localMessage));
    DestroyMessage(&localMessage);
}

TEST(MessageTestGroup, AuthHeaderParseTest)
{
    struct Message *message = CreateMessage();

    ParseMessage(UNAUTHORIZED_MESSAGE, message);
    struct AuthHeader *auth = (struct AuthHeader *)MessageGetHeader(HEADER_NAME_WWW_AUTHENTICATE, message);
    struct Parameters *ps = AuthHeaderGetParameters(auth);

    STRCMP_EQUAL("MD5", GetParameter(ps, (char *)"algorithm"));
    STRCMP_EQUAL("\"asterisk\"", GetParameter(ps, (char *)"realm"));
    STRCMP_EQUAL("\"1cd2586e\"", GetParameter(ps, (char *)"nonce"));
    
    DestroyMessage(&message);
}
