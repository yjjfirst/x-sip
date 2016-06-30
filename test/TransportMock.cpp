#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TransportMock.h"

extern "C" {
#include <stdio.h>
#include <string.h>
#include "MessageTransport.h"
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
