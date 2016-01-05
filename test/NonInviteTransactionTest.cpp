#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TestingMessages.h"
#include "TransportMock.h"

extern "C" {
#include <stdio.h>
#include <string.h>

#include "Transaction.h"
#include "MessageBuilder.h"
#include "Messages.h"
#include "MessageTransport.h"
#include "Header.h"
#include "Parameter.h"
#include "ViaHeader.h"
#include "TransactionManager.h"
#include "UserAgent.h"
#include "StatusLine.h"
#include "Dialog.h"
}

static TimerCallback TimerECallbackFunc;
static TimerCallback TimerFCallbackFunc;
static TimerCallback TimerKCallbackFunc;

struct Timer *AddTimerMock(void *p, int ms, TimerCallback onTime)
{
    mock().actualCall("AddTimer").withIntParameter("ms", ms);
    if (TimerECallbackFunc == NULL)
        TimerECallbackFunc = onTime;
    else if (TimerFCallbackFunc == NULL)
        TimerFCallbackFunc = onTime;
    else
        TimerKCallbackFunc = onTime;
    return NULL;
}

TEST_GROUP(TransactionTestGroup)
{
    struct Message *m;
    struct Transaction *t;
    enum TransactionState s;
    struct UserAgent *ua;
    struct Dialog *dialog;

    void setup()
    {
        UT_PTR_SET(Transporter, &MockTransporter);
        UT_PTR_SET(AddTimer, AddTimerMock);
        UT_PTR_SET(ReceiveMessageCallback, MessageReceived);

        mock().expectOneCall("AddTimer").withIntParameter("ms", T1);
        mock().expectOneCall("AddTimer").withIntParameter("ms", 64*T1);
        mock().expectOneCall("SendOutMessageMock");

        ua = CreateUserAgent();
        dialog = CreateDialog(NULL, ua);
        m = BuildBindingMessage(dialog);
        t = AddClientTransaction(m, NULL);
        s = TransactionGetState(t);
    }

    void teardown()
    {
        EmptyTransactionManager();
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

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(BINDING_TRYING_MESSAGE);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);
    ReceiveInMessage(string);
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, s);

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(BINDING_TRYING_MESSAGE);
    ReceiveInMessage(string);
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, s);

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(ADD_BINDING_OK_MESSAGE);
    mock().expectOneCall("AddTimer").withIntParameter("ms", T4);
    ReceiveInMessage(string);
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, s);
}

TEST(TransactionTestGroup, Receive2xxTest)
{
    char string[MAX_MESSAGE_LENGTH] = {0};

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(ADD_BINDING_OK_MESSAGE);
    mock().expectOneCall("AddTimer").withIntParameter("ms", T4);

    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);

    ReceiveInMessage(string);
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, s);
}

TEST(TransactionTestGroup, TryingTimeETest)
{
    mock().expectOneCall("AddTimer").withIntParameter("ms", 2*T1);
    mock().expectOneCall("SendOutMessageMock");
    TimerECallbackFunc(t);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);
    mock().checkExpectations();

    mock().expectOneCall("AddTimer").withIntParameter("ms", 4*T1);
    mock().expectOneCall("SendOutMessageMock");
    TimerECallbackFunc(t);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);
    mock().checkExpectations();

    mock().expectOneCall("AddTimer").withIntParameter("ms", 8*T1);
    mock().expectOneCall("SendOutMessageMock");
    TimerECallbackFunc(t);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);
    mock().checkExpectations();

    mock().expectOneCall("AddTimer").withIntParameter("ms", T4);
    mock().expectOneCall("SendOutMessageMock");
    TimerECallbackFunc(t);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);
    mock().checkExpectations();

    mock().expectOneCall("AddTimer").withIntParameter("ms", T4);
    mock().expectOneCall("SendOutMessageMock");
    TimerECallbackFunc(t);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);
    mock().checkExpectations();

    mock().expectOneCall("AddTimer").withIntParameter("ms", T4);
    mock().expectOneCall("SendOutMessageMock");
    TimerECallbackFunc(t);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);
    mock().checkExpectations();
}

TEST(TransactionTestGroup, ProceedingTimeETest)
{
    char string[MAX_MESSAGE_LENGTH] = {0};

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(BINDING_TRYING_MESSAGE);

    ReceiveInMessage(string);
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, s);

    mock().expectOneCall("AddTimer").withIntParameter("ms",2*T1);
    mock().expectOneCall("SendOutMessageMock");
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

    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);
    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(ADD_BINDING_OK_MESSAGE);   
    mock().expectOneCall("AddTimer").withIntParameter("ms", T4);

    ReceiveInMessage(string);
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, s);

    TimerKCallbackFunc(t);
    POINTERS_EQUAL(NULL, GetTransactionBy((char *)"z9hG4bK1491280923", (char *)SIP_METHOD_NAME_REGISTER));
}

TEST(TransactionTestGroup, SendOutMessageError)
{
    RemoveTransaction(t);
    
    mock().expectOneCall("AddTimer").withIntParameter("ms", T1);
    mock().expectOneCall("AddTimer").withIntParameter("ms", 64*T1);
    mock().expectOneCall("SendOutMessageMock").andReturnValue(-1);

    struct Message *message = BuildBindingMessage(dialog);
    AddClientTransaction(message, NULL);

    POINTERS_EQUAL(NULL, GetTransactionBy((char *)"z9hG4bK1491280923", (char *)SIP_METHOD_NAME_REGISTER));

    DestoryUserAgent(&ua);
    EmptyTransactionManager();
}

TEST(TransactionTestGroup, ProceedingTransportError)
{
    char string[MAX_MESSAGE_LENGTH] = {0};

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(BINDING_TRYING_MESSAGE);

    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);

    ReceiveInMessage(string);
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, s);
    
    mock().expectOneCall("SendOutMessageMock").andReturnValue(-1);
    TimerECallbackFunc(t);
    POINTERS_EQUAL(NULL, GetTransactionBy((char *)"z9hG4bK1491280923", (char *)SIP_METHOD_NAME_REGISTER));
    CHECK_EQUAL(0, CountTransaction());
    mock().checkExpectations();
}

TEST(TransactionTestGroup, TryingTransportErrorTest)
{
    mock().expectOneCall("SendOutMessageMock").andReturnValue(-1);
    TimerECallbackFunc(t);
    POINTERS_EQUAL(NULL, GetTransactionBy((char *)"z9hG4bK1491280923", (char *)SIP_METHOD_NAME_REGISTER));
    CHECK_EQUAL(0, CountTransaction());    
    mock().checkExpectations();
}

TEST(TransactionTestGroup, GetLatestResponse)
{
    char string[MAX_MESSAGE_LENGTH] = {0};

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(BINDING_TRYING_MESSAGE);
    ReceiveInMessage(string);

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(BINDING_TRYING_MESSAGE);
    ReceiveInMessage(string);

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(ADD_BINDING_OK_MESSAGE);
    mock().expectOneCall("AddTimer").withIntParameter("ms", T4);
    ReceiveInMessage(string);

    struct Message *latestResponse = TransactionGetLatestResponse(t);
    struct StatusLine *sl = MessageGetStatusLine(latestResponse);
    CHECK_EQUAL(200,StatusLineGetStatusCode(sl));
}
