#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "Mock.h"

extern "C" {
#include "Transporter.h"
#include "SipUdp.h"
#include "Udp.h"
}

TEST_GROUP(TransporterUdpTestGroup)
{
    void setup()
    {
        UT_PTR_SET(xsocket, socket_mock);
        UT_PTR_SET(xbind, bind_mock);
        UT_PTR_SET(xsendto, sendto_mock);
        UT_PTR_SET(xrecvfrom, recvfrom_mock);
    }
    void teardown()
    {
        mock().checkExpectations();
        mock().clear();
    }
};

TEST(TransporterUdpTestGroup, BindFailedTest)
{
    mock().expectOneCall("socket").andReturnValue(0);
    mock().expectOneCall("bind").andReturnValue(-1);

    CHECK_EQUAL(-1, SipUdpInit(0));
}


TEST(TransporterUdpTestGroup, CreateSocketFailedTest)
{
    mock().expectOneCall("socket").andReturnValue(-1);
    CHECK_EQUAL(-1, SipUdpInit(0));
}

TEST(TransporterUdpTestGroup, InitSuccessedTest)
{
    mock().expectOneCall("socket").andReturnValue(0);
    mock().expectOneCall("bind").andReturnValue(0);

    CHECK_EQUAL(0, SipUdpInit(0));
}

TEST(TransporterUdpTestGroup, SendMessageTest)
{
    char message[] = "Testing message";
    
    mock().expectOneCall("sendto");
    SipUdpSendMessage(message, NULL, 0, 0);
}

TEST(TransporterUdpTestGroup, ReceiveMessageTest)
{
    char message[64] ={0};
    
    mock().expectOneCall("recvfrom");
    SipUdpReceiveMessage(message, 0);
}
