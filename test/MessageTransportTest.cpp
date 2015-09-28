#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include <string.h>
#include "Messages.h"
#include "MessageTransport.h"
}

TEST_GROUP(MessageTransportTestGroup)
{
    void teardown() {
        mock().clear();
    }
};

int ReceiveMessageMock(char *message)
{
    strcpy(message, "asdf");
    mock().actualCall("ReceiveMessageMock");
    return 0;
}

int SendMessageMock(char *message)
{
    return 0;
}
void InitMessageTransport()
{
    AddMessageTransport((char *)"Mock", SendMessageMock, ReceiveMessageMock);
}

TEST(MessageTransportTestGroup, ReceiveMessageTest)
{
    char message[36] = {0};

    InitMessageTransport();
    mock().expectOneCall("ReceiveMessageMock");
    ReceiveMessage(message);
    STRCMP_EQUAL("asdf", message);
    mock().checkExpectations();
}
