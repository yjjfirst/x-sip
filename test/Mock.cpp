#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "Mock.h"

extern "C" {
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "Transporter.h"
#include "Messages.h"
#include "RequestLine.h"
#include "StatusLine.h"
#include "TransactionManager.h"
}

int ReceiveMessageMock(char *message, int fd)
{
    strcpy(message, mock().actualCall(RECEIVE_IN_MESSAGE_MOCK).returnStringValue());
    return 0;
}

int SendMessageMock(char *message, char *destaddr, int destport,  int fd)
{
    MESSAGE *m = CreateMessage();
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

int SendOutMessageMockForMessageDestTest(char *message, char *destaddr, int destport,  int fd)
{
    MESSAGE *m = CreateMessage();
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
            withStringParameter("destaddr", destaddr).
            withIntParameter("port", destport).
            returnIntValue();
    } else { 
        statusCode = StatusLineGetStatusCode(MessageGetStatusLine(m));
        DestroyMessage(&m);
        return mock().actualCall(SEND_OUT_MESSAGE_MOCK).
            withIntParameter("StatusCode", statusCode).
            withStringParameter("destaddr", destaddr).
            withIntParameter("destport", destport).
            returnIntValue();
    }
}

int InitMock(int port)
{
    return mock().actualCall("InitMock").withIntParameter("port", port).returnIntValue();
}

void NotifyCallManagerMock(int event, struct UserAgent *ua)
{
    mock().actualCall("NotifyCm").withParameter("event", event).
        withParameter("ua", ua);
}

void OnTransactionEventMock(struct Dialog *dialog,  int event, MESSAGE *message)
{
    mock().actualCall("OnTransactionEvent").withParameter("event", event);
}

int MessageHandleMock(char *message, char *ipAddr, int port)
{
    return mock().actualCall("MessageHandleMock").returnIntValue();
}

struct MessageTransporter MockTransporter = {
    .send     = SendMessageMock,
    .receive  = ReceiveMessageMock,
    .init     = InitMock,
    .callback = SipMessageCallback,
};

struct MessageTransporter MockTransporterAndHandle = {
    .send     = SendOutMessageMockForMessageDestTest,
    .receive  = ReceiveMessageMock,
    .init     = InitMock,
    .callback = MessageHandleMock,
};

int DummySend(char *message, char *destaddr, int destport, int fd)
{
    return 0;
}

int DummyReceive(char *message, int fd)
{
    return 0;
}

struct MessageTransporter DummyTransporter = {
    .send = DummySend,
    .receive = DummyReceive,
    .init = NULL,
    .callback = SipMessageCallback
};

int SendClientMessageMock(char *message, char *destaddr, int destport, int fd)
{
    mock().actualCall("SendEventToClient").withStringParameter("message", message);
    return 0;
}

struct MessageTransporter ClientTransporterMock = {
    .send = SendClientMessageMock,
    .receive = NULL,
    .init = NULL,
    .callback = NULL,
};

void GenerateBranchMock(char *branch)
{
    strcpy(branch, "z9hG4bK1491280923");
}

void GenerateBranchMockForAuthorization(char *branch)
{
    strcpy(branch, "z9hG4bK1491280927");
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
