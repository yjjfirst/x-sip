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
#include "DialogManager.h"
}

#define INVITE_100TRYING_MESSAGE (char *)"\
SIP/2.0 100 Trying\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5061;branch=z9hG4bK1491280923;received=192.168.10.1;rport=5061\r\n\
From: <sip:88001@192.168.10.62>;tag=1226271270\r\n\
To: <sip:88002@192.168.10.62>\r\n\
Call-ID: 778885328\r\n\
CSeq: 20 INVITE\r\n\
Contact: <sip:88002@192.168.10.62:5060>\r\n\
Content-Length: 0\r\n"

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

void OnEventMock(struct Dialog *dialog, int event, MESSAGE *message)
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
        UT_PTR_SET(SipTransporter, &MockTransporter);
        UT_PTR_SET(GenerateBranch, GenerateBranchMock);
        
        ExpectedNewClientTransaction(SIP_METHOD_INVITE);
        AccountInit();
        ua = CreateUserAgent(0);
        dialog = AddDialog(NULL_DIALOG_ID, ua);
        message = BuildInviteMessage((char *)"88001", (char *)"88002", (char *)"192.168.10.62", 5060); 

        t = DialogNewTransaction(dialog, message, TRANSACTION_TYPE_CLIENT_INVITE);

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
        
        ReceiveMessage();    
        CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, GetTransactionState(t));

    }

    void PrepareCompletedState()
    {
        mock().expectOneCall("AddTimer").withParameter("ms", WAIT_TIME_FOR_RESPONSE_RETRANSMITS);        
        Receive3xxResponse(t);
        CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, GetTransactionState(t));
    }
};

TEST(ClientInviteTransactionTestGroup, GetTransactionTest)
{
    struct TransactionId *id = GetTransactionId(t);
    (void) id;
}
//Init tests.
TEST(ClientInviteTransactionTestGroup, CreateInviteTransaction)
{ 
    CHECK_EQUAL(TRANSACTION_STATE_CALLING, GetTransactionState(t));
}

static struct Session *CreateSessionMock()
{
    return NULL;
}

//Calling state tests.
TEST(ClientInviteTransactionTestGroup, CallingStateReceive2xxTest)
{
    UT_PTR_SET(CreateSession, CreateSessionMock);
    
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(OK_MESSAGE_RECEIVED);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", SIP_METHOD_NAME_ACK);

    ReceiveMessage();

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
    ReceiveMessage();
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, GetTransactionState(t));

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(RINGING_MESSAGE_RECEIVED);
    ReceiveMessage();    
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, GetTransactionState(t));
}

TEST(ClientInviteTransactionTestGroup, CallingStateReceive180Without100ReceivedTest)
{
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(RINGING_MESSAGE_RECEIVED);
    ReceiveMessage();    
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, GetTransactionState(t));
}

TEST(ClientInviteTransactionTestGroup, CallingStateReveive180NotifyUserTest)
{
    UT_PTR_SET(OnTransactionEvent, OnEventMock);

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_100TRYING_MESSAGE);
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(RINGING_MESSAGE_RECEIVED);
    mock().expectOneCall("NotifyUser");
    
    ReceiveMessage();
    ReceiveMessage();    
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
        CHECK_EQUAL(TRANSACTION_STATE_CALLING, GetTransactionState(t));
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
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, GetTransactionState(t));
}

TEST(ClientInviteTransactionTestGroup, ProceedingState2xxReceiveTest)
{
    PrepareProceedingState();

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(OK_MESSAGE_RECEIVED);

    ReceiveMessage();    
    POINTERS_EQUAL(NULL, GetTransaction(branch, (char *)SIP_METHOD_NAME_INVITE));
 
}

TEST(ClientInviteTransactionTestGroup, ProceedingState1xxReceiveTest)
{
    int i = 0;

    for (; i < 20; i++) {
        mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_100TRYING_MESSAGE);
        ReceiveMessage();
        CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, GetTransactionState(t));
    }
}

TEST(ClientInviteTransactionTestGroup, ProceedingState5xxReceiveTest)
{
    PrepareProceedingState();
    RunFsmByStatusCode(t, 503);

    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, GetTransactionState(t));
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
        CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, GetTransactionState(t));
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
