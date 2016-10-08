#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "Mock.h"

extern "C" {
#include <stdio.h>
#include "TransporterManager.h"
#include "Transporter.h"
#include "SipUdp.h"
#include "Udp.h"
#include "ClientUdp.h"
}

TEST_GROUP(TransporterManagerTestGroup)
{
    void teardown()
    {
        ClearTransporterManager();
    }
};

int ReceiveMessageMock(char *message, int fd)
{
    mock().actualCall("ReceiveMessage").withParameter("fd", fd);
    return 0;
}

struct MessageTransporter t0 = {NULL, ReceiveMessageMock, NULL, NULL, 0};
struct MessageTransporter t1 = {NULL, ReceiveMessageMock, NULL, NULL, 101};
struct MessageTransporter t2 = {NULL, ReceiveMessageMock, NULL, NULL, 102};
struct MessageTransporter t3 = {NULL, ReceiveMessageMock, NULL, NULL, 103};
struct MessageTransporter t4 = {NULL, ReceiveMessageMock, NULL, NULL, 104};
struct MessageTransporter t5 = {NULL, ReceiveMessageMock, NULL, NULL, 1023};

TEST(TransporterManagerTestGroup, AddTransporterTest)
{
    AddTransporter(&t1);
    CHECK_EQUAL(1, CountTransporter());

    AddTransporter(&t2);
    CHECK_EQUAL(2, CountTransporter());

    AddTransporter(&t3);
    CHECK_EQUAL(3, CountTransporter());

    AddTransporter(&t4);
    CHECK_EQUAL(4, CountTransporter());
}

TEST(TransporterManagerTestGroup, ClearTransporterTest)
{
    AddTransporter(&t1);
    AddTransporter(&t2);
    AddTransporter(&t3);
    AddTransporter(&t4);

    ClearTransporterManager();
    CHECK_EQUAL(0, CountTransporter());
}

TEST(TransporterManagerTestGroup, GetTransporterTest)
{
    AddTransporter(&t1);
    AddTransporter(&t2);
    AddTransporter(&t3);
    AddTransporter(&t4);

    POINTERS_EQUAL(&t1, GetTransporter(101));
    POINTERS_EQUAL(&t2, GetTransporter(102));
    POINTERS_EQUAL(&t3, GetTransporter(103));
    POINTERS_EQUAL(&t4, GetTransporter(104));
}

TEST(TransporterManagerTestGroup, InitTest)
{
    UT_PTR_SET(xbind, bind_mock);

    mock().expectOneCall("bind").andReturnValue(100);
    mock().expectOneCall("bind").andReturnValue(200);

    TransporterManagerInit();    
    
    POINTERS_EQUAL(&SipUdpTransporter, GetTransporter(SipUdpTransporter.fd));
    POINTERS_EQUAL(&ClientUdpTransporter, GetTransporter(ClientUdpTransporter.fd));

    mock().clear();
}

TEST(TransporterManagerTestGroup, MaxFdTest)
{
    AddTransporter(&t1);
    AddTransporter(&t2);
    AddTransporter(&t3);
    AddTransporter(&t4);
    CHECK_EQUAL(104, GetMaxFd());

    AddTransporter(&t5);
    CHECK_EQUAL(1023, GetMaxFd());
}

TEST(TransporterManagerTestGroup, FD_SET_FillTest)
{
    fd_set fdsr;
    int i;
    
    AddTransporter(&t0);
    AddTransporter(&t1);
    AddTransporter(&t2);
    AddTransporter(&t3);
    AddTransporter(&t4);
    AddTransporter(&t5);

    FillFdset(&fdsr);

    CHECK_TRUE(FD_ISSET(0, &fdsr));
    CHECK_TRUE(FD_ISSET(101, &fdsr));
    CHECK_TRUE(FD_ISSET(102, &fdsr));
    CHECK_TRUE(FD_ISSET(103, &fdsr));
    CHECK_TRUE(FD_ISSET(104, &fdsr));
    CHECK_TRUE(FD_ISSET(1023, &fdsr));
    
    for (i = 1; i <= 100; i++) {
        CHECK_FALSE(FD_ISSET(i, &fdsr));
    }

    for (i = 105; i < 1023; i++) {
        CHECK_FALSE(FD_ISSET(i, &fdsr));
    }

}

TEST(TransporterManagerTestGroup, ReceiveMessagesTest)
{
    fd_set fdsr;

    AddTransporter(&t0);
    AddTransporter(&t1);
    AddTransporter(&t2);
    AddTransporter(&t3);
    AddTransporter(&t4);
    AddTransporter(&t5);
    FillFdset(&fdsr);

    mock().expectOneCall("ReceiveMessage").withParameter("fd", 0);
    mock().expectOneCall("ReceiveMessage").withParameter("fd", 101);
    mock().expectOneCall("ReceiveMessage").withParameter("fd", 102);
    mock().expectOneCall("ReceiveMessage").withParameter("fd", 103);
    mock().expectOneCall("ReceiveMessage").withParameter("fd", 104);
    mock().expectOneCall("ReceiveMessage").withParameter("fd", 1023);
   
    ReceiveMessages(&fdsr);

    mock().checkExpectations();
    mock().clear();
}

TEST(TransporterManagerTestGroup, ReceiveMessagesNoFdSetTest)
{
    fd_set fdsr;

    AddTransporter(&t0);
    AddTransporter(&t1);
    AddTransporter(&t2);
    AddTransporter(&t3);
    AddTransporter(&t4);
    AddTransporter(&t5);

    FillFdset(&fdsr);
    FD_ZERO(&fdsr);
    
    ReceiveMessages(&fdsr);

    mock().checkExpectations();
    mock().clear();
}
