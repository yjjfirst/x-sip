#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "Mock.h"
#include "TestingMessages.h"

extern "C" {
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "Messages.h"
#include "Transporter.h"
#include "TransactionManager.h"
#include "Transaction.h"
#include "UserAgent.h"
#include "StatusLine.h"
#include "UserAgentManager.h"
#include "Dialog.h"
}

TimerCallback RetransmitTimerAction;
TimerCallback TimeOutTimerAction; 
TimerCallback WaitAckTimerAction;

int SendOut100TryingMock(char *message, char *destaddr, int destport, int fd)
{
    MESSAGE *m = CreateMessage();
    ParseMessage(message, m);
    struct StatusLine *sl = MessageGetStatusLine(m);
    
    CHECK_EQUAL(100, StatusLineGetStatusCode(sl));
    STRCMP_EQUAL("Trying", StatusLineGetReasonPhrase(sl));

    DestroyMessage(&m);

    return mock().actualCall(SEND_OUT_MESSAGE_MOCK).returnIntValue();    
}

struct MessageTransporter MockTransporterFor100Trying = {
    SendOut100TryingMock,
    ReceiveInMessageMock,
    NULL,
    SipMessageHandle
};

int SendOut180RingingMock(char *message, char *destaddr, int destport, int fd)
{

    MESSAGE *m = CreateMessage();
    ParseMessage(message, m);
    struct StatusLine *sl = MessageGetStatusLine(m);

    CHECK_EQUAL(180, StatusLineGetStatusCode(sl));
    STRCMP_EQUAL("Ringing", StatusLineGetReasonPhrase(sl));
    DestroyMessage(&m);

    return mock().actualCall(SEND_OUT_MESSAGE_MOCK).returnIntValue();    
}

struct MessageTransporter MockTransporterFor180Ringing = {
    SendOut180RingingMock,
    ReceiveInMessageMock,
    NULL,
    SipMessageHandle
};

int SendOut301Mock(char *message, char *destaddr, int destport, int fd)
{

    MESSAGE *m = CreateMessage();
    ParseMessage(message, m);
    struct StatusLine *sl = MessageGetStatusLine(m);

    CHECK_EQUAL(STATUS_CODE_MOVED_PERMANENTLY, StatusLineGetStatusCode(sl));
    STRCMP_EQUAL(REASON_PHRASE_MOVED_PERMANENTLY, StatusLineGetReasonPhrase(sl));
    DestroyMessage(&m);

    return mock().actualCall(SEND_OUT_MESSAGE_MOCK).returnIntValue();    
}

struct MessageTransporter MockTransporterFor301 = {
    SendOut301Mock,
    ReceiveInMessageMock,
    NULL,
    SipMessageHandle
};

int SendOut200OKMock(char *message, char *destaddr, int destport, int fd)
{

    MESSAGE *m = CreateMessage();
    ParseMessage(message, m);
    struct StatusLine *sl = MessageGetStatusLine(m);

    CHECK_EQUAL(200, StatusLineGetStatusCode(sl));
    STRCMP_EQUAL("OK", StatusLineGetReasonPhrase(sl));
    DestroyMessage(&m);
    return mock().actualCall(SEND_OUT_MESSAGE_MOCK).returnIntValue();    
}

struct MessageTransporter MockTransporterFor200OK = {
    SendOut200OKMock,
    ReceiveInMessageMock,
    NULL,
    SipMessageHandle
};

static struct Timer *AddTimerMock(void *data, int interval, TimerCallback action)
{
    if (interval == WAIT_TIME_FOR_ACK_RECEIPT) {
        TimeOutTimerAction = action;
    } else if (interval == WAIT_TIME_FOR_ACK_RETRANSMITS) {
        WaitAckTimerAction = action;
    } else {
        RetransmitTimerAction = action;
    }
    mock().actualCall("AddTimerMock").withParameter("interval", interval);
    return NULL;
}

TEST_GROUP(ServerInviteTransactionTestGroup)
{
    void setup(){
        UT_PTR_SET(SipTransporter, &MockTransporter);
    }

    void teardown() {
        ClearTransactionManager();
        mock().checkExpectations();
        mock().clear();
    }
    
    void CheckOnlyOneTransactionMatched()
    {
        struct Transaction *t = NULL;
        t = GetTransaction((char *)"z9hG4bK27dc30b4",(char *)"INVITE");
        
        CHECK_EQUAL(1, CountTransaction());
        CHECK_TRUE(t != NULL);
        CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, GetTransactionState(t));
    }

    void CheckNoTransaction()
    {
        struct Transaction *t = NULL;
        t = GetTransaction((char *)"z9hG4bK27dc30b4",(char *)"INVITE");
        CHECK_EQUAL(0, t);
    }

    struct Transaction *PrepareProceedingState()
    {
        struct TransactionUser *user = NULL;
        MESSAGE *request = CreateMessage();

        mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
        ParseMessage(INCOMMING_INVITE_MESSAGE, request);
        struct Transaction *t = AddTransaction(request, user, TRANSACTION_TYPE_SERVER_INVITE);

        return t;
    }

    struct Transaction *PrepareCompletedState()
    {
        struct Transaction *t = PrepareProceedingState();

        UT_PTR_SET(AddTimer, AddTimerMock);
        mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
        mock().expectOneCall("AddTimerMock").withIntParameter("interval", INITIAL_REQUEST_RETRANSMIT_INTERVAL);
        mock().expectOneCall("AddTimerMock").withIntParameter("interval", WAIT_TIME_FOR_ACK_RECEIPT);

        UT_PTR_SET(SipTransporter, &MockTransporterFor301);    
        ResponseWith301(t);
        CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, GetTransactionState(t)); 

        return t;
    }
    
    struct Transaction *PrepareConfirmedState()
    {
        struct Transaction *t = PrepareCompletedState();
        
        mock().expectOneCall("AddTimerMock").withParameter("interval", WAIT_TIME_FOR_ACK_RETRANSMITS);
        ReceiveAckRequest(t); 
        CHECK_EQUAL(TRANSACTION_STATE_CONFIRMED, GetTransactionState(t));
        
        return t;
    }
};

//Create transaction tests.
TEST(ServerInviteTransactionTestGroup, CreateTransactionTest)
{

    UT_PTR_SET(SipTransporter, &MockTransporterFor100Trying);

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
    ReceiveInMessage(); 
    CheckOnlyOneTransactionMatched();
    ClearUserAgentManager();
}

TEST(ServerInviteTransactionTestGroup, CreateTransactionTransportErrorTest)
{
    struct TransactionUser *user = NULL;
    MESSAGE *request = CreateMessage();

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK)
        .withIntParameter("StatusCode", 100)
        .andReturnValue(-1);
    ParseMessage(INCOMMING_INVITE_MESSAGE, request);
    AddTransaction(request, user, TRANSACTION_TYPE_SERVER_INVITE);

    CheckNoTransaction();
}

//Proceeding state tests.
TEST(ServerInviteTransactionTestGroup, ProceedingStateReceiveInviteTest)
{
    PrepareProceedingState();

    CheckOnlyOneTransactionMatched();

    //Receive retransmited INVITE message.
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    ReceiveInMessage(); 
    CheckOnlyOneTransactionMatched();
    
    //Receive retransmited INVITE message.
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    ReceiveInMessage(); 
    CheckOnlyOneTransactionMatched();
}

TEST(ServerInviteTransactionTestGroup, ProceedingStateSend180FromTuTest)
{
    struct Transaction *t = PrepareProceedingState();
    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
    UT_PTR_SET(SipTransporter, &MockTransporterFor180Ringing);
    ResponseWith180Ringing(t);

    CHECK_EQUAL(1, CountTransaction());
    CHECK_TRUE(t != NULL);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, GetTransactionState(t));
}

TEST(ServerInviteTransactionTestGroup, ProceedingStateSend180ErrorTest)
{
    struct Transaction *t = PrepareProceedingState();
    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).andReturnValue(-1);
    UT_PTR_SET(SipTransporter, &MockTransporterFor180Ringing);
    ResponseWith180Ringing(t);

    CheckNoTransaction();
}

TEST(ServerInviteTransactionTestGroup, ProceedingStateSend200OKFromTuTest)
{
    struct Transaction *t = PrepareProceedingState();

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
    UT_PTR_SET(SipTransporter, &MockTransporterFor200OK);    
    ResponseWith200OK(t);
    
    CheckNoTransaction();
}

TEST(ServerInviteTransactionTestGroup, ProceedingStateSend200OKSendErrorTest)
{
    struct Transaction *t = PrepareProceedingState();

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).andReturnValue(-1);
    UT_PTR_SET(SipTransporter, &MockTransporterFor200OK);    
    ResponseWith200OK(t);
    
    CheckNoTransaction();
}

TEST(ServerInviteTransactionTestGroup, ProceedingStateSend301FromTuTest)
{
    struct Transaction *t = PrepareProceedingState();

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
    UT_PTR_SET(SipTransporter, &MockTransporterFor301);    
    ResponseWith301(t);

    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, GetTransactionState(t)); 
}

TEST(ServerInviteTransactionTestGroup, ProceedingStateSend301FromTuAddTimerTest)
{
    struct Transaction *t = PrepareProceedingState();

    mock().expectOneCall("AddTimerMock").withIntParameter("interval", INITIAL_REQUEST_RETRANSMIT_INTERVAL);
    mock().expectOneCall("AddTimerMock").withIntParameter("interval", TRANSACTION_TIMEOUT_INTERVAL);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK)
        .withIntParameter("StatusCode", 301)
        .andReturnValue(0);    
    UT_PTR_SET(AddTimer, AddTimerMock);

    ResponseWith301(t);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, GetTransactionState(t)); 
}

TEST(ServerInviteTransactionTestGroup, ProceedingState301TransportErrorTest)
{
    struct Transaction *t = PrepareProceedingState();

    UT_PTR_SET(SipTransporter, &MockTransporterFor301);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).andReturnValue(-1);    
    ResponseWith301(t);
 
    CheckNoTransaction();
}

//Completed State tests
TEST(ServerInviteTransactionTestGroup, CompletedStateReceiceInviteStateTest)
{
    struct Transaction *t = PrepareCompletedState();

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
    ReceiveInMessage();
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, GetTransactionState(t)); 

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
    ReceiveInMessage();
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, GetTransactionState(t)); 

}

TEST(ServerInviteTransactionTestGroup, CompletedStateSendResponseErrorTest)
{
    PrepareCompletedState();

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).andReturnValue(-1);
    ReceiveInMessage();

    CheckNoTransaction(); 
}

TEST(ServerInviteTransactionTestGroup, CompletedStateRetransmitTest)
{
    struct Transaction *t = PrepareCompletedState();

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);    
    RetransmitTimerAction(t);

    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, GetTransactionState(t)); 
}

TEST(ServerInviteTransactionTestGroup, CompletedStateRetransmitErrorTest)
{
    struct Transaction *t = PrepareCompletedState();

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).andReturnValue(-1);
    RetransmitTimerAction(t);
    
    CheckNoTransaction(); 
}

TEST(ServerInviteTransactionTestGroup, CompletedStateTimeoutTimerTest)
{
    struct Transaction *t = PrepareCompletedState();

    TimeOutTimerAction(t);
    CheckNoTransaction();
}

TEST(ServerInviteTransactionTestGroup, CompletedStateReceiveAckTest)
{
    PrepareConfirmedState();
}

//Confirmed state tests
TEST(ServerInviteTransactionTestGroup, ConfirmedStateWaitForAckTimerTest)
{
    struct Transaction *t = PrepareConfirmedState();
    WaitAckTimerAction(t);
    CheckNoTransaction();
}

TEST(ServerInviteTransactionTestGroup, ReceiveInviteTest)
{
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("StatusCode", 100);
    ReceiveInMessage();

    CheckOnlyOneTransactionMatched();
    ClearUserAgentManager();    
}
