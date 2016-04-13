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

TEST_GROUP(ClientNotInviteTransactionTestGroup)
{
    struct Message *m;
    struct Transaction *t;
    enum TransactionState s;
    struct UserAgent *ua;
    struct Dialog *dialog;

    struct Transaction *PrepareTryingState(int sendExpected)
    {
        if (sendExpected != -1) {
            mock().expectOneCall("AddTimer").withIntParameter("ms", INITIAL_REQUEST_RETRANSMIT_INTERVAL);
            mock().expectOneCall("AddTimer").withIntParameter("ms", TRANSACTION_TIMEOUT_INTERVAL);
        }

        mock().expectOneCall(SEND_OUT_MESSAGE_MOCK)
            .withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER))
            .andReturnValue(sendExpected);
    
        t = AddClientNonInviteTransaction(m, NULL);
        if ( t != NULL)
            CHECK_EQUAL(TRANSACTION_STATE_TRYING, TransactionGetState(t));

        return t;
    }

    void PrepareProceedingState()
    {
        char string[MAX_MESSAGE_LENGTH] = {0};
        
        mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(BINDING_TRYING_MESSAGE);
        ReceiveInMessage(string);
        s = TransactionGetState(t);
        CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, s);
        
    }

    void CheckNoTransaction()
    {
        POINTERS_EQUAL(NULL, GetTransaction((char *)"z9hG4bK1491280923", (char *)SIP_METHOD_NAME_REGISTER));
    }

    void setup()
    {
        UT_PTR_SET(Transporter, &MockTransporter);
        UT_PTR_SET(AddTimer, AddTimerMock);
        UT_PTR_SET(ReceiveMessageCallback, MessageReceived);

        ua = CreateUserAgent();
        dialog = CreateDialog(NULL_DIALOG_ID, ua);
        m = BuildBindingMessage(dialog);
    }

    void teardown()
    {
        EmptyTransactionManager();
        DestoryUserAgent(&ua);
        mock().checkExpectations();
        mock().clear();
    }
};

//Trying state tests
TEST(ClientNotInviteTransactionTestGroup, TryingStateInitTest)
{
    PrepareTryingState(0);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, TransactionGetState(t));
}

TEST(ClientNotInviteTransactionTestGroup, TryingStateReceive1xxTest)
{
    PrepareTryingState(0);
    PrepareProceedingState(); 
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));
}

TEST(ClientNotInviteTransactionTestGroup, TryingStateReceive2xxTest)
{
    char string[MAX_MESSAGE_LENGTH] = {0};

    PrepareTryingState(0);
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(ADD_BINDING_OK_MESSAGE);
    mock().expectOneCall("AddTimer").withIntParameter("ms", WAIT_TIME_FOR_RESPONSE_RETRANSMITS);

    ReceiveInMessage(string);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t));
}

TEST(ClientNotInviteTransactionTestGroup, TryingStateTimeETest)
{
    int i = 0;
    
    PrepareTryingState(0);

    for (; i < 20; i ++) {
        int expected = (2<<i)*INITIAL_REQUEST_RETRANSMIT_INTERVAL;        
        if (expected > MAXIMUM_RETRANSMIT_INTERVAL) expected = MAXIMUM_RETRANSMIT_INTERVAL;

        mock().expectOneCall("AddTimer").withIntParameter("ms", expected);
        mock().expectOneCall(SEND_OUT_MESSAGE_MOCK)
            .withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER));
        TimerECallbackFunc(t);
        CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);
        mock().checkExpectations();
    }
}

TEST(ClientNotInviteTransactionTestGroup, TryingStateTimerFTest)
{
    PrepareTryingState(0);
    TimerFCallbackFunc(t);
    CheckNoTransaction();
}

TEST(ClientNotInviteTransactionTestGroup, TryingStateEnterSendOutMessageError)
{
    PrepareTryingState(-1);
    POINTERS_EQUAL(NULL, GetTransaction((char *)"z9hG4bK1491280923", (char *)SIP_METHOD_NAME_REGISTER));
}

TEST(ClientNotInviteTransactionTestGroup, TryingStateResendErrorTest)
{
    PrepareTryingState(0);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK)
        .withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER))
        .andReturnValue(-1);
    TimerECallbackFunc(t);

    POINTERS_EQUAL(NULL, GetTransaction((char *)"z9hG4bK1491280923", (char *)SIP_METHOD_NAME_REGISTER));
    CHECK_EQUAL(0, CountTransaction());    
}

//Proceeding State tests.
TEST(ClientNotInviteTransactionTestGroup, ProceedingStateReceiveMulti1xxTest)
{
    int i = 0;

    PrepareTryingState(0);

    for ( ; i < 20; i++) {
        PrepareProceedingState(); 
        PrepareProceedingState(); 
        PrepareProceedingState(); 
        PrepareProceedingState();
    } 
}

TEST(ClientNotInviteTransactionTestGroup, ProceedingStateTimeETest)
{
    int i = 0;

    PrepareTryingState(0);
    PrepareProceedingState();

    for (; i < 20; i++) {
        mock().expectOneCall("AddTimer").withIntParameter("ms",MAXIMUM_RETRANSMIT_INTERVAL);
        mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).
            withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER));
        TimerECallbackFunc(t);
        CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, s);
        mock().checkExpectations();
    }
}

TEST(ClientNotInviteTransactionTestGroup, ProceedingStateTimeFTest) 
{
    PrepareTryingState(0);
    PrepareProceedingState();
    TimerFCallbackFunc(t);
    CheckNoTransaction();
}

TEST(ClientNotInviteTransactionTestGroup, ProceedingStateResendErrorTest)
{
    PrepareTryingState(0);
    PrepareProceedingState();
    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK)
        .withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER))
        .andReturnValue(-1);
    TimerECallbackFunc(t);

    POINTERS_EQUAL(NULL, GetTransaction((char *)"z9hG4bK1491280923", (char *)SIP_METHOD_NAME_REGISTER));
    CHECK_EQUAL(0, CountTransaction());
}

//Completed state
TEST(ClientNotInviteTransactionTestGroup, CompletedStateTimerKTest)
{
    char string[MAX_MESSAGE_LENGTH] = {0};
    
    PrepareTryingState(0);
    PrepareProceedingState();

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(ADD_BINDING_OK_MESSAGE);   
    mock().expectOneCall("AddTimer").withIntParameter("ms", WAIT_TIME_FOR_RESPONSE_RETRANSMITS);

    ReceiveInMessage(string);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t));

    TimerKCallbackFunc(t);
    POINTERS_EQUAL(NULL, GetTransaction((char *)"z9hG4bK1491280923", (char *)SIP_METHOD_NAME_REGISTER));
}

//Other tests.
TEST(ClientNotInviteTransactionTestGroup, GetLatestResponse)
{
    char string[MAX_MESSAGE_LENGTH] = {0};

    PrepareTryingState(0);

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(BINDING_TRYING_MESSAGE);
    ReceiveInMessage(string);

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(BINDING_TRYING_MESSAGE);
    ReceiveInMessage(string);

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(ADD_BINDING_OK_MESSAGE);
    mock().expectOneCall("AddTimer").withIntParameter("ms", WAIT_TIME_FOR_RESPONSE_RETRANSMITS);
    ReceiveInMessage(string);

    struct Message *latestResponse = TransactionGetLatestResponse(t);
    struct StatusLine *sl = MessageGetStatusLine(latestResponse);
    CHECK_EQUAL(200,StatusLineGetStatusCode(sl));
}
