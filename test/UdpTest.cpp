#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include "Transporter.h"
#include "Udp.h"
}
int bind_mock(int sockfd, const struct sockaddr *addr,
         socklen_t addrlen)
{
    return mock().actualCall("bind").returnIntValue();
}

int socket_mock(int domain, int type, int protocol)
{
    return mock().actualCall("socket").returnIntValue();
}

ssize_t sendto_mock(int sockfd, const void *buf, size_t len, int flags,
                    const struct sockaddr *dest_addr, socklen_t addrlen)
{
    return mock().actualCall("sendto").returnIntValue();
}
ssize_t recvfrom_mock(int sockfd, void *buf, size_t len, int flags,
                     struct sockaddr *src_addr, socklen_t *addrlen)
{
    return mock().actualCall("recvfrom").returnIntValue();
}

void CreateRecvTaskMock(){}

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

    CHECK_EQUAL(-1, UdpTransporterInit());
}


TEST(TransporterUdpTestGroup, CreateSocketFailedTest)
{
    mock().expectOneCall("socket").andReturnValue(-1);
    CHECK_EQUAL(-1, UdpTransporterInit());
}

TEST(TransporterUdpTestGroup, InitSuccessedTest)
{
    mock().expectOneCall("socket").andReturnValue(0);
    mock().expectOneCall("bind").andReturnValue(0);

    CHECK_EQUAL(0, UdpTransporterInit());
}

TEST(TransporterUdpTestGroup, SendMessageTest)
{
    char message[] = "Testing message";
    
    mock().expectOneCall("sendto");
    UdpSendMessage(message);
}

TEST(TransporterUdpTestGroup, ReceiveMessageTest)
{
    char message[64] ={0};
    
    mock().expectOneCall("recvfrom");
    UdpReceiveMessage(message);
}
