#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TestingMessages.h"

extern "C" {
#include <stdio.h>
#include <string.h>

#include "Timer.h"
#include "Transaction.h"
#include "MessageBuilder.h"
#include "Messages.h"
#include "MessageTransport.h"
#include "Header.h"
#include "Parameter.h"
#include "ViaHeader.h"
#include "TransactionManager.h"
#include "UserAgent.h"
}

enum Response {
    OK200,
    RINGING180,
};

static enum Response Response;
static TimerCallback TimerECallbackFunc;
static TimerCallback TimerFCallbackFunc;
static TimerCallback TimerKCallbackFunc;

int TransactionReceiveMessageMock(char *message)
{
    if (Response == RINGING180)
        strcpy(message, TryingMessage);
    else
        strcpy(message, OKMessage);
    return 0;
}

int TransactionSendMessageMock(char *message)
{
    return mock().actualCall("TransactionSendMessageMock").returnIntValue();
}

void AddTimer(void *p, int ms, TimerCallback onTime)
{
    mock().actualCall("AddTimer").withIntParameter("ms", ms);
    if (TimerECallbackFunc == NULL)
        TimerECallbackFunc = onTime;
    else if (TimerFCallbackFunc == NULL)
        TimerFCallbackFunc = onTime;
    else
        TimerKCallbackFunc = onTime;
}

TEST_GROUP(TransactionTestGroup)
{
    struct Message *m;
    struct Transaction *t;
    enum TransactionState s;
    struct TransactionManager *manager;
    struct UserAgent *ua;

    void setup()
    {
        mock().expectOneCall("AddTimer").withIntParameter("ms", T1);
        mock().expectOneCall("AddTimer").withIntParameter("ms", 64*T1);
        mock().expectOneCall("TransactionSendMessageMock");

        RemoveMessageTransporter((char *)"TRANS");
        AddMessageTransporter((char *)"TRANS", TransactionSendMessageMock, TransactionReceiveMessageMock);
        TransactionSetTimer(AddTimer);
        ua = CreateUserAgent();

        m = BuildRegisterMessage(ua);
        manager = GetTransactionManager();
        t = CreateTransactionExt(m, NULL);
        s = TransactionGetState(t);
        InitReceiveMessageCallback(MessageReceived);
    }

    void teardown()
    {
        RemoveMessageTransporter((char *)"TRANS");
        DestoryTransactionManager();
        DestoryUserAgent(&ua);
        mock().clear();
    }
};

TEST(TransactionTestGroup, TransactionInitTest)
{
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);
}

TEST(TransactionTestGroup, Receive1xxTest)
{
    char string[MAX_MESSAGE_LENGTH] = {0};

    Response = RINGING180;

    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);


    ReceiveMessage(string);
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, s);

    ReceiveMessage(string);
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, s);

    Response = OK200;
    mock().expectOneCall("AddTimer").withIntParameter("ms", T4);
    ReceiveMessage(string);
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, s);
}

TEST(TransactionTestGroup, Receive2xxTest)
{
    char string[MAX_MESSAGE_LENGTH] = {0};

    Response = OK200;
    mock().expectOneCall("AddTimer").withIntParameter("ms", T4);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);

    ReceiveMessage(string);
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, s);
}

TEST(TransactionTestGroup, TryingTimeETest)
{
    mock().expectOneCall("AddTimer").withIntParameter("ms", 2*T1);
    mock().expectOneCall("TransactionSendMessageMock");
    TimerECallbackFunc(t);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);
    mock().checkExpectations();

    mock().expectOneCall("AddTimer").withIntParameter("ms", 4*T1);
    mock().expectOneCall("TransactionSendMessageMock");
    TimerECallbackFunc(t);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);
    mock().checkExpectations();

    mock().expectOneCall("AddTimer").withIntParameter("ms", 8*T1);
    mock().expectOneCall("TransactionSendMessageMock");
    TimerECallbackFunc(t);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);
    mock().checkExpectations();

    mock().expectOneCall("AddTimer").withIntParameter("ms", T4);
    mock().expectOneCall("TransactionSendMessageMock");
    TimerECallbackFunc(t);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);
    mock().checkExpectations();

    mock().expectOneCall("AddTimer").withIntParameter("ms", T4);
    mock().expectOneCall("TransactionSendMessageMock");
    TimerECallbackFunc(t);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);
    mock().checkExpectations();

    mock().expectOneCall("AddTimer").withIntParameter("ms", T4);
    mock().expectOneCall("TransactionSendMessageMock");
    TimerECallbackFunc(t);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);
    mock().checkExpectations();
}

TEST(TransactionTestGroup, ProceedingTimeETest)
{
    char string[MAX_MESSAGE_LENGTH] = {0};

    Response = RINGING180;

    ReceiveMessage(string);
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, s);

    mock().expectOneCall("AddTimer").withIntParameter("ms",2*T1);
    mock().expectOneCall("TransactionSendMessageMock");
    TimerECallbackFunc(t);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, s);
    mock().checkExpectations();
}

TEST(TransactionTestGroup, TimerFTest)
{

    POINTERS_EQUAL(t, GetTransactionBy((char *)"z9hG4bK1491280923", (char *)SIP_METHOD_NAME_REGISTER));
    TimerFCallbackFunc(t);
    s = TransactionGetState(t);

    POINTERS_EQUAL(NULL, GetTransactionBy((char *)"z9hG4bK1491280923", (char *)SIP_METHOD_NAME_REGISTER));
}

TEST(TransactionTestGroup, TimerKTest)
{
    char string[MAX_MESSAGE_LENGTH] = {0};

    Response = OK200;
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);

    mock().expectOneCall("AddTimer").withIntParameter("ms", T4);
    ReceiveMessage(string);
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, s);

    TimerKCallbackFunc(t);
    POINTERS_EQUAL(NULL, GetTransactionBy((char *)"z9hG4bK1491280923", (char *)SIP_METHOD_NAME_REGISTER));
}

TEST(TransactionTestGroup, SendMessageError)
{
    RemoveTransaction(t);
    
    mock().expectOneCall("AddTimer").withIntParameter("ms", T1);
    mock().expectOneCall("AddTimer").withIntParameter("ms", 64*T1);
    mock().expectOneCall("TransactionSendMessageMock").andReturnValue(-1);

    struct UserAgent *ua = CreateUserAgent();
    struct Message *message = BuildRegisterMessage(ua);
    CreateTransactionExt(message, NULL);

    POINTERS_EQUAL(NULL, GetTransactionBy((char *)"z9hG4bK1491280923", (char *)SIP_METHOD_NAME_REGISTER));

    DestoryUserAgent(&ua);
    DestoryTransactionManager();
}

TEST(TransactionTestGroup, ProceedingTransportError)
{
    char string[MAX_MESSAGE_LENGTH] = {0};

    Response = RINGING180;

    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);

    ReceiveMessage(string);
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, s);
    
    mock().expectOneCall("TransactionSendMessageMock").andReturnValue(-1);
    TimerECallbackFunc(t);
    POINTERS_EQUAL(NULL, GetTransactionBy((char *)"z9hG4bK1491280923", (char *)SIP_METHOD_NAME_REGISTER));
    CHECK_EQUAL(0, CountTransaction());
    mock().checkExpectations();
}

TEST(TransactionTestGroup, TryingTransportErrorTest)
{
    mock().expectOneCall("TransactionSendMessageMock").andReturnValue(-1);
    TimerECallbackFunc(t);
    POINTERS_EQUAL(NULL, GetTransactionBy((char *)"z9hG4bK1491280923", (char *)SIP_METHOD_NAME_REGISTER));
    CHECK_EQUAL(0, CountTransaction());    
    mock().checkExpectations();
}
