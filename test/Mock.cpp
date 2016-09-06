#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "Mock.h"

extern "C" {
#include <stdio.h>
#include <string.h>
#include "Transporter.h"
#include "Messages.h"
#include "RequestLine.h"
#include "StatusLine.h"
}

int ReceiveInMessageMock(char *message)
{
    strcpy(message, mock().actualCall(RECEIVE_IN_MESSAGE_MOCK).returnStringValue());
    return 0;
}

int SendOutMessageMock(char *message)
{
    struct Message *m = CreateMessage();
    ParseMessage(message, m);
    SIP_METHOD method;
    int statusCode;
    enum MESSAGE_TYPE type;

    type = MessageGetType(m);
    if (type == MESSAGE_TYPE_REQUEST) {
        method = RequestLineGetMethod(MessageGetRequestLine(m));
        DestroyMessage(&m);
        return mock().actualCall(SEND_OUT_MESSAGE_MOCK).
            withStringParameter("Method", MethodMap2String(method)).
            returnIntValue();
    } else { 
        statusCode = StatusLineGetStatusCode(MessageGetStatusLine(m));
        DestroyMessage(&m);
        return mock().actualCall(SEND_OUT_MESSAGE_MOCK).
            withIntParameter("StatusCode", statusCode).
            returnIntValue();
    }
}

struct MessageTransporter MockTransporter = {
    SendOutMessageMock,
    ReceiveInMessageMock,
};

int DummySend(char *message)
{
    return 0;
}

int DummyReceive(char *message)
{
    return 0;
}

struct MessageTransporter DummyTransporter = {
    .send = DummySend,
    .receive = DummyReceive,
};

void GenerateBranchMock(char *branch)
{
    strcpy(branch, "z9hG4bK1491280923");
}

void GenerateBranchMockForAuthorization(char *branch)
{
    strcpy(branch, "z9hG4bK1491280927");
}
