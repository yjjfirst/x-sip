#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TestingMessages.h"
#include "Mock.h"

extern "C" {
#include <stdio.h>
#include <string.h>

#include "Transaction.h"
#include "MessageBuilder.h"
#include "Messages.h"
#include "Transporter.h"
#include "Header.h"
#include "Parameter.h"
#include "ViaHeader.h"
#include "TransactionManager.h"
#include "UserAgent.h"
#include "StatusLine.h"
#include "Dialog.h"
#include "AccountManager.h"
#include "ViaHeader.h"
#include "DialogManager.h"
#include "Accounts.h"
}

#define  BINDING_TRYING_MESSAGE (char *)"\
SIP/2.0 100 Trying\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5060;branch=z9hG4bK1491280923;received=192.168.10.1;rport=5060\r\n\
From: <sip:88001@192.168.10.62>;tag=1225432999\r\n\
To: <sip:88001@192.168.10.62>;tag=as1d07559a\r\n\
Call-ID: 1222971951\r\n\
CSeq: 1 REGISTER\r\n\
Expires:3600\r\n\
Contact: <sip:88001@192.168.10.1;line=f2fd53ebfa7728f>;expires=3600\r\n\
Content-Length: 0\r\n"

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

TEST_GROUP(ClientNonInviteTransactionTestGroup)
{
    MESSAGE *m;
    struct Transaction *t;
    enum TransactionState state;
    struct UserAgent *ua;
    struct Dialog *dialog;
    char branch[64];
    
    struct Transaction *PrepareTryingState(int sendExpected)
    {
        if (sendExpected != -1) {
            mock().expectOneCall("AddTimer").withIntParameter("ms", INITIAL_REQUEST_RETRANSMIT_INTERVAL);
            mock().expectOneCall("AddTimer").withIntParameter("ms", TRANSACTION_TIMEOUT_INTERVAL);
        }

        mock().expectOneCall(SEND_OUT_MESSAGE_MOCK)
            .withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER))
            .andReturnValue(sendExpected);

        t = DialogNewTransaction(dialog, m, TRANSACTION_TYPE_CLIENT_NON_INVITE);

        return t;
    }

    void PrepareProceedingState()
    {
        mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(BINDING_TRYING_MESSAGE);
        ReceiveMessage();
        state = GetTransactionState(t);
        CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, state);
        
    }

    void CheckNoTransaction()
    {
        POINTERS_EQUAL(NULL, GetTransaction(branch, (char *)SIP_METHOD_NAME_REGISTER));
    }

    void setup()
    {
        UT_PTR_SET(SipTransporter, &MockTransporter);
        UT_PTR_SET(AddTimer, AddTimerMock);
        UT_PTR_SET(GenerateBranch, GenerateBranchMock);

        AccountInit();
        ua = CreateUserAgent(0);
        dialog = AddDialog(NULL_DIALOG_ID, ua);
        m = BuildAddBindingMessage(
            AccountGetUserName(GetAccount(0)),
            AccountGetUserName(GetAccount(0)),
            AccountGetProxyAddr(GetAccount(0)),
            AccountGetProxyPort(GetAccount(0)));
        
        strcpy(branch, MessageGetViaBranch(m));
    }

    void teardown()
    {
        ClearAccountManager();
        ClearTransactionManager();
        DestroyUserAgent(&ua);
        mock().checkExpectations();
        mock().clear();
    }
};

//Trying state tests
TEST(ClientNonInviteTransactionTestGroup, TryingStateInitTest)
{
    PrepareTryingState(0);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, GetTransactionState(t));
}

TEST(ClientNonInviteTransactionTestGroup, TryingStateReceive1xxTest)
{
    PrepareTryingState(0);
    PrepareProceedingState(); 
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, GetTransactionState(t));
}

TEST(ClientNonInviteTransactionTestGroup, TryingStateReceive2xxTest)
{
    PrepareTryingState(0);
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(ADD_BINDING_OK_MESSAGE);
    mock().expectOneCall("AddTimer").withIntParameter("ms", WAIT_TIME_FOR_RESPONSE_RETRANSMITS);

    ReceiveMessage();
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, GetTransactionState(t));
}

TEST(ClientNonInviteTransactionTestGroup, TryStateReceive401Test)
{
    PrepareTryingState(0);
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(UNAUTHORIZED_MESSAGE);
    mock().expectOneCall("AddTimer").withIntParameter("ms", WAIT_TIME_FOR_RESPONSE_RETRANSMITS);
    
    mock().expectOneCall("AddTimer").withIntParameter("ms", INITIAL_REQUEST_RETRANSMIT_INTERVAL);
    mock().expectOneCall("AddTimer").withIntParameter("ms", TRANSACTION_TIMEOUT_INTERVAL);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER));
 
    
    ReceiveMessage();
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, GetTransactionState(t));
}

TEST(ClientNonInviteTransactionTestGroup, TryingStateTimeETest)
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
        
        CHECK_EQUAL(TRANSACTION_STATE_TRYING, GetTransactionState(t));
        mock().checkExpectations();
    }
}

TEST(ClientNonInviteTransactionTestGroup, TryingStateTimerFTest)
{
    PrepareTryingState(0);
    TimerFCallbackFunc(t);
    CheckNoTransaction();
}

TEST(ClientNonInviteTransactionTestGroup, TryingStateEnterSendOutMessageError)
{
    PrepareTryingState(-1);
//    DumpTransactionManager();
    POINTERS_EQUAL(NULL, GetTransaction(branch, (char *)SIP_METHOD_NAME_REGISTER));
}

TEST(ClientNonInviteTransactionTestGroup, TryingStateResendErrorTest)
{
    PrepareTryingState(0);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK)
        .withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER))
        .andReturnValue(-1);
    TimerECallbackFunc(t);

    POINTERS_EQUAL(NULL, GetTransaction(branch, (char *)SIP_METHOD_NAME_REGISTER));
    CHECK_EQUAL(0, CountTransaction());    
}

//Proceeding State tests.
TEST(ClientNonInviteTransactionTestGroup, ProceedingStateReceiveMulti1xxTest)
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

TEST(ClientNonInviteTransactionTestGroup, ProceedingStateTimeETest)
{
    int i = 0;

    PrepareTryingState(0);
    PrepareProceedingState();

    for (; i < 20; i++) {
        mock().expectOneCall("AddTimer").withIntParameter("ms",MAXIMUM_RETRANSMIT_INTERVAL);
        mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).
            withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER));
        TimerECallbackFunc(t);
        CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, GetTransactionState(t));
        mock().checkExpectations();
    }
}

TEST(ClientNonInviteTransactionTestGroup, ProceedingStateTimeFTest) 
{
    PrepareTryingState(0);
    PrepareProceedingState();
    TimerFCallbackFunc(t);
    CheckNoTransaction();
}

TEST(ClientNonInviteTransactionTestGroup, ProceedingStateResendErrorTest)
{
    PrepareTryingState(0);
    PrepareProceedingState();
    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK)
        .withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER))
        .andReturnValue(-1);
    TimerECallbackFunc(t);

    POINTERS_EQUAL(NULL, GetTransaction(branch, (char *)SIP_METHOD_NAME_REGISTER));
    CHECK_EQUAL(0, CountTransaction());
}

//Completed state
TEST(ClientNonInviteTransactionTestGroup, CompletedStateTimerKTest)
{
    PrepareTryingState(0);
    PrepareProceedingState();

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(ADD_BINDING_OK_MESSAGE);   
    mock().expectOneCall("AddTimer").withIntParameter("ms", WAIT_TIME_FOR_RESPONSE_RETRANSMITS);

    ReceiveMessage();
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, GetTransactionState(t));

    TimerKCallbackFunc(t);
    POINTERS_EQUAL(NULL, GetTransaction(branch, (char *)SIP_METHOD_NAME_REGISTER));
}

//Other tests.
TEST(ClientNonInviteTransactionTestGroup, GetLatestResponse)
{
    PrepareTryingState(0);

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(BINDING_TRYING_MESSAGE);
    ReceiveMessage();

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(BINDING_TRYING_MESSAGE);
    ReceiveMessage();

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(ADD_BINDING_OK_MESSAGE);
    mock().expectOneCall("AddTimer").withIntParameter("ms", WAIT_TIME_FOR_RESPONSE_RETRANSMITS);
    ReceiveMessage();

    MESSAGE *latestResponse = GetLatestResponse(t);
    struct StatusLine *sl = MessageGetStatusLine(latestResponse);
    CHECK_EQUAL(200,GetStatusCode(sl));
}
