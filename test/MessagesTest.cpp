#include "CppUTest/TestHarness.h"

extern "C" {
#include <stdio.h>
#include <strings.h>
#include <string.h>

#include "Parser.h"
#include "Messages.h"
#include "ViaHeader.h"
#include "MaxForwards.h"
}

TEST_GROUP(MessageTestGroup)
{
    char messageString[2048];
    void setup()
    {
        strcpy(messageString, " \
     REGISTER sip:registrar.munich.de SIP/2.0 \r\n                      \
     Via: SIP/2.0/UDP 200.201.202.203:5060;branch=z9hG4bKus19 \r\n      \
     Max-Forwards: 70 \r\n                                              \
     To: Werner Heisenberg <sip:werner.heisenberg@munich.de> \r\n       \
     From: Werner Heisenberg <sip:werner.heisenberg@munich.de> ;tag=3431 \r\n \
     Call-ID: 23@200.201.202.203 \r\n                                   \
     CSeq: 1 REGISTER \r\n                                              \
     Contact: sip:werner.heisenberg@200.201.202.203 \r\n                \
     Content-Length: 0 \r\n");
    }
};

TEST(MessageTestGroup, RegisterRequestLineParseTest)
{
    
    struct Message *message = CreateMessage();
    ParseMessage(messageString, message);
    
    struct RequestLine *request = MessageGetRequest(message);
    STRCMP_EQUAL("REGISTER", RequestLineGetMethod(request));
    STRCMP_EQUAL("SIP/2.0", RequestLineGetSipVersion(request));
    
    struct URI *uri = RequestLineGetUri(request);
    STRCMP_EQUAL("sip", UriGetScheme(uri));
    STRCMP_EQUAL("registrar.munich.de", UriGetHost(uri));

    DestoryMessage(&message);
}

TEST(MessageTestGroup, ViaHeaderParseTest)
{
    struct Message *message = CreateMessage();
    ParseMessage(messageString, message);

    struct Header *header =  MessageGetHeader("Via", message);
    struct ViaHeader *via = (struct ViaHeader*)header;

    STRCMP_EQUAL("Via", ViaHeaderGetName(via));
    STRCMP_EQUAL("SIP/2.0/UDP", ViaHeaderGetTransport(via));
    STRCMP_EQUAL("200.201.202.203:5060", ViaHeaderGetUri(via));
    STRCMP_EQUAL("branch=z9hG4bKus19", ViaHeaderGetParameters(via));
    DestoryMessage(&message);    
}

TEST(MessageTestGroup, MaxForwardsParseTest)
{
    struct Message *message = CreateMessage();
    ParseMessage(messageString, message);

    struct MaxForwardsHeader *m = (struct MaxForwardsHeader *)MessageGetHeader("Max-Forwards", message);
    
    STRCMP_EQUAL("Max-Forwards", MaxForwardsGetName(m));
    CHECK_EQUAL(70, MaxForwardsGetMaxForwards(m));
    DestoryMessage(&message);    

}

TEST(MessageTestGroup, ExtractHeaderNameTest)
{
    char header[] = "Via: SIP/2.0/UDP 200.201.202.203:5060;branch=z9hG4bKus19";
    char name[32] = {0};

    ExtractHeaderName(header, name);
    STRCMP_EQUAL("Via", name);
}
