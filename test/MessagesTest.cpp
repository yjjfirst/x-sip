#include "CppUTest/TestHarness.h"

extern "C" {
#include <stdio.h>
#include <strings.h>
#include "Messages.h"
}
TEST_GROUP(MessageTestGroup)
{
};

TEST(MessageTestGroup, RegisterParseTest)
{
    char messageString[] = " \
     REGISTER sip:registrar.munich.de SIP/2.0 \r\n                 \
     Via: SIP/2.0/UDP 200.201.202.203:5060;branch=z9hG4bKus19 \r\n \
     Max-Forwards: 70 \r\n                                         \
     To: Werner Heisenberg <sip:werner.heisenberg@munich.de> \r\n       \
     From: Werner Heisenberg <sip:werner.heisenberg@munich.de> ;tag=3431 \r\n \
     Call-ID: 23@200.201.202.203 \r\n                                   \
     CSeq: 1 REGISTER \r\n                                              \
     Contact: sip:werner.heisenberg@200.201.202.203 \r\n                \
     Content-Length: 0 \r\n" ;
    
    struct Message *message = CreateMessage();
    ParseMessage(messageString, message);
    
    struct RequestLine *request = MessageGetRequest(message);
    STRCMP_EQUAL("REGISTER", RequestLineGetMethod(request));

    DestoryMessage(message);
}

