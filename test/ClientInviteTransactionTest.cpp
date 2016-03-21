#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TransportMock.h"

extern "C" {
#include <stdio.h>
#include <string.h>
#include "Messages.h"
#include "MessageBuilder.h"
#include "UserAgent.h"
#include "TransactionManager.h"
#include "Transaction.h"
#include "MessageTransport.h"
#include "TestingMessages.h"
#include "CallIdHeader.h"
#include "Dialog.h"
#include "Provision.h"
#include "UserAgentManager.h"
#include "Timer.h"
}

TimerCallback TimerACallbackFunc;
TimerCallback TimerBCallbackFunc;
TimerCallback TimerDCallbackFunc;

static struct Timer *AddTimerMock(void *p, int interval, TimerCallback action)
{
    mock().actualCall("AddTimer").withParameter("ms", interval);

    if (interval == TRANSACTION_TIMEOUT_INTERVAL) {
        TimerBCallbackFunc = action;
    } else if (interval == WAIT_TIME_FOR_RESPONSE_RETRANSMITS) {
        TimerDCallbackFunc = action;
    } else {
        TimerACallbackFunc = action;
    }
    return NULL;
}

TEST_GROUP(ClientInviteTransactionTestGroup)
{
    struct UserAgent *ua;
    struct Message *message;
    struct Transaction *t;
    struct Dialog *dialog;
    void setup(){
        UT_PTR_SET(ReceiveMessageCallback, MessageReceived);
        UT_PTR_SET(AddTimer, AddTimerMock);
        UT_PTR_SET(Transporter, &MockTransporter);

        ExpectedNewClientTransaction();
        ua = BuildUserAgent();
        dialog = CreateDialog(NULL_DIALOG_ID, ua);
        message = BuildInviteMessage(dialog); 
        t = AddClientInviteTransaction(message,(struct TransactionUserNotifiers *) dialog);

    }

    void teardown() {
        mock().checkExpectations();
        mock().clear();

        DestoryUserAgent(&ua);
        EmptyTransactionManager();
    }
    
    void ExpectedNewClientTransaction()
    {
        mock().expectOneCall("AddTimer").withParameter("ms", INITIAL_REQUEST_RETRANSMIT_INTERVAL);
        mock().expectOneCall("AddTimer").withParameter("ms", TRANSACTION_TIMEOUT_INTERVAL);
        mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
    }

    void PrepareProceedingState()
    {
        char stringReceived[MAX_MESSAGE_LENGTH] = {0};

        mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_100TRYING_MESSAGE);
        
        ReceiveInMessage(stringReceived);    
        CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));

    }

    void PrepareCompletedState()
    {
        mock().expectOneCall("AddTimer").withParameter("ms", WAIT_TIME_FOR_RESPONSE_RETRANSMITS);        
        Receive3xxResponse(t);
        CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t));
    }
};
//Init tests.
TEST(ClientInviteTransactionTestGroup, CreateInviteTransaction)
{ 
    CHECK_EQUAL(TRANSACTION_STATE_CALLING, TransactionGetState(t));
}

//Calling state tests.
TEST(ClientInviteTransactionTestGroup, CallingStateReceive2xxTest)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_200OK_MESSAGE);

    ExpectedNewClientTransaction();
    ReceiveInMessage(stringReceived);    
    POINTERS_EQUAL(NULL, GetTransaction((char *)"z9hG4bK1491280923", (char *)SIP_METHOD_NAME_INVITE));
}

TEST(ClientInviteTransactionTestGroup, CallingStateReceive100Test)
{
    PrepareProceedingState();
}

TEST(ClientInviteTransactionTestGroup, CallingStateReceive180Test)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_100TRYING_MESSAGE);
    ReceiveInMessage(stringReceived);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_180RINGING_MESSAGE);
    ReceiveInMessage(stringReceived);    
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));
}

TEST(ClientInviteTransactionTestGroup, CallingStateTimerATest)
{
    int i = 0;

    for (; i < 20; i ++) {
        int expected = (2<<i)*INITIAL_REQUEST_RETRANSMIT_INTERVAL;        
        if (expected > MAXIMUM_RETRANSMIT_INTERVAL) expected = MAXIMUM_RETRANSMIT_INTERVAL;

        mock().expectOneCall("AddTimer").withIntParameter("ms", expected);
        mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
        TimerACallbackFunc(t);
        CHECK_EQUAL(TRANSACTION_STATE_CALLING, TransactionGetState(t));
        mock().checkExpectations();
    }
}

TEST(ClientInviteTransactionTestGroup, CallingStateTimerBTest)
{
    TimerBCallbackFunc(t);
    POINTERS_EQUAL(NULL, GetTransaction((char *)"z9hG4bK1491280923", (char *)SIP_METHOD_NAME_INVITE));
}

TEST(ClientInviteTransactionTestGroup, CallingState3xxReceiveTest)
{
    PrepareCompletedState();
}

//Proceeding state.
TEST(ClientInviteTransactionTestGroup, ProceedingState3xxReceiveTest)
{
    PrepareProceedingState();
    mock().expectOneCall("AddTimer").withParameter("ms", WAIT_TIME_FOR_RESPONSE_RETRANSMITS);
    Receive3xxResponse(t);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t));
}

TEST(ClientInviteTransactionTestGroup, ProceedingState2xxReceiveTest)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};
    
    PrepareProceedingState();

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_200OK_MESSAGE);

    ReceiveInMessage(stringReceived);    
    POINTERS_EQUAL(NULL, GetTransaction((char *)"z9hG4bK1491280923", (char *)SIP_METHOD_NAME_INVITE));
 
}

TEST(ClientInviteTransactionTestGroup, ProceedingState1xxReceiveTest)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};
    int i = 0;

    for (; i < 20; i++) {
        mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_100TRYING_MESSAGE);
        ReceiveInMessage(stringReceived);
        CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));
    }
}

//Completed state
TEST(ClientInviteTransactionTestGroup, CompletedStateReceive3xxTest)
{
    PrepareCompletedState();
    mock().checkExpectations();

    int i = 0;

    for (;i < 20; i++) {
        mock().expectOneCall("SendOutMessageMock");
        Receive3xxResponse(t);
        CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t));
    }
}

TEST(ClientInviteTransactionTestGroup, CompletedStateTransportErrorTest)
{
    PrepareCompletedState();

    mock().expectOneCall("SendOutMessageMock").andReturnValue(-1);
    Receive3xxResponse(t);
    POINTERS_EQUAL(NULL, GetTransaction((char *)"z9hG4bK1491280923", (char *)SIP_METHOD_NAME_INVITE));    
}

TEST(ClientInviteTransactionTestGroup, CompletedStateTimerDTest)
{
    PrepareCompletedState();
    TimerDCallbackFunc(t);
    POINTERS_EQUAL(NULL, GetTransaction((char *)"z9hG4bK1491280923", (char *)SIP_METHOD_NAME_INVITE));    
}
