#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "Mock.h"

extern "C" {
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "Messages.h"
#include "MessageBuilder.h"
#include "UserAgent.h"
#include "TransactionManager.h"
#include "Transaction.h"
#include "Transporter.h"
#include "TestingMessages.h"
#include "CallIdHeader.h"
#include "Dialog.h"
#include "Provision.h"
#include "UserAgentManager.h"
#include "Timer.h"
#include "Session.h"
#include "AccountManager.h"
#include "ViaHeader.h"
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

void OnEventMock(struct Transaction *t)
{    
    mock().actualCall("NotifyUser");
}

TEST_GROUP(ClientInviteTransactionTestGroup)
{
    struct UserAgent *ua;
    MESSAGE *message;
    struct Transaction *t;
    struct Dialog *dialog;

    char branch[64];
    void setup(){
        UT_PTR_SET(AddTimer, AddTimerMock);
        UT_PTR_SET(Transporter, &MockTransporter);
        UT_PTR_SET(GenerateBranch, GenerateBranchMock);
        
        ExpectedNewClientTransaction(SIP_METHOD_INVITE);
        AccountInit();
        ua = CreateUserAgent(0);
        dialog = AddNewDialog(NULL_DIALOG_ID, ua);
        message = BuildInviteMessage(dialog, (char *)"88002"); 
        t = AddClientInviteTransaction(message,(struct TransactionUser *) dialog);

        strcpy(branch, MessageGetViaBranch(message));
    }

    void teardown() {
        DestroyUserAgent(&ua);
        ClearTransactionManager();
        ClearAccountManager();
        
        mock().checkExpectations();
        mock().clear();
    }
    
    void ExpectedNewClientTransaction(SIP_METHOD method)
    {
        mock().expectOneCall("AddTimer").withParameter("ms", INITIAL_REQUEST_RETRANSMIT_INTERVAL);
        mock().expectOneCall("AddTimer").withParameter("ms", TRANSACTION_TIMEOUT_INTERVAL);
        mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(method));
    }

    void PrepareProceedingState()
    {
        mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_100TRYING_MESSAGE);
        
        ReceiveInMessage();    
        CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));

    }

    void PrepareCompletedState()
    {
        mock().expectOneCall("AddTimer").withParameter("ms", WAIT_TIME_FOR_RESPONSE_RETRANSMITS);        
        Receive3xxResponse(t);
        CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t));
    }
};

TEST(ClientInviteTransactionTestGroup, GetTransactionTest)
{
    struct TransactionId *id = TransactionGetId(t);
    (void) id;
}
//Init tests.
TEST(ClientInviteTransactionTestGroup, CreateInviteTransaction)
{ 
    CHECK_EQUAL(TRANSACTION_STATE_CALLING, TransactionGetState(t));
}

static struct Session *CreateSessionMock()
{
    return NULL;
}

//Calling state tests.
TEST(ClientInviteTransactionTestGroup, CallingStateReceive2xxTest)
{
    UT_PTR_SET(CreateSession, CreateSessionMock);
    
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_200OK_MESSAGE);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", SIP_METHOD_NAME_ACK);

    ReceiveInMessage();

    POINTERS_EQUAL(NULL, GetTransaction(branch, (char *)SIP_METHOD_NAME_INVITE));
    CHECK_EQUAL(0, CountTransaction());
}

TEST(ClientInviteTransactionTestGroup, CallingStateReceive100Test)
{
    PrepareProceedingState();
}

TEST(ClientInviteTransactionTestGroup, CallingStateReceive180Test)
{
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_100TRYING_MESSAGE);
    ReceiveInMessage();
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_180RINGING_MESSAGE);
    ReceiveInMessage();    
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));
}

TEST(ClientInviteTransactionTestGroup, CallingStateReceive180Without100ReceivedTest)
{
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_180RINGING_MESSAGE);
    ReceiveInMessage();    
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));
}

TEST(ClientInviteTransactionTestGroup, CallingStateReveive180NotifyUserTest)
{
    UT_PTR_SET(OnTransactionEvent, OnEventMock);

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_100TRYING_MESSAGE);
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_180RINGING_MESSAGE);
    mock().expectOneCall("NotifyUser");
    
    ReceiveInMessage();
    ReceiveInMessage();    
}

TEST(ClientInviteTransactionTestGroup, CallingStateTimerATest)
{
    int i = 0;

    for (; i < 20; i ++) {
        int expected = (2<<i)*INITIAL_REQUEST_RETRANSMIT_INTERVAL;        
        if (expected > MAXIMUM_RETRANSMIT_INTERVAL) expected = MAXIMUM_RETRANSMIT_INTERVAL;

        mock().expectOneCall("AddTimer").withIntParameter("ms", expected);
        mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_INVITE));
        TimerACallbackFunc(t);
        CHECK_EQUAL(TRANSACTION_STATE_CALLING, TransactionGetState(t));
        mock().checkExpectations();
    }
}

TEST(ClientInviteTransactionTestGroup, CallingStateTimerBTest)
{
    TimerBCallbackFunc(t);
    POINTERS_EQUAL(NULL, GetTransaction(branch, (char *)SIP_METHOD_NAME_INVITE));
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
    PrepareProceedingState();

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_200OK_MESSAGE);

    ReceiveInMessage();    
    POINTERS_EQUAL(NULL, GetTransaction(branch, (char *)SIP_METHOD_NAME_INVITE));
 
}

TEST(ClientInviteTransactionTestGroup, ProceedingState1xxReceiveTest)
{
    int i = 0;

    for (; i < 20; i++) {
        mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_100TRYING_MESSAGE);
        ReceiveInMessage();
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
        mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).
            withStringParameter("Method", MethodMap2String(SIP_METHOD_ACK));
        Receive3xxResponse(t);
        CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t));
    }
}

TEST(ClientInviteTransactionTestGroup, CompletedStateTransportErrorTest)
{
    PrepareCompletedState();

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", SIP_METHOD_NAME_ACK).andReturnValue(-1);
    Receive3xxResponse(t);
    POINTERS_EQUAL(NULL, GetTransaction(branch, (char *)SIP_METHOD_NAME_INVITE));    
}

TEST(ClientInviteTransactionTestGroup, CompletedStateTimerDTest)
{
    PrepareCompletedState();
    TimerDCallbackFunc(t);
    POINTERS_EQUAL(NULL, GetTransaction(branch, (char *)SIP_METHOD_NAME_INVITE));    
}
