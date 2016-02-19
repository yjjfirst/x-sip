#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TransportMock.h"
#include "TestingMessages.h"

extern "C" {
#include <stdio.h>
#include <assert.h>
#include "Messages.h"
#include "MessageTransport.h"
#include "TransactionManager.h"
#include "Transaction.h"
#include "UserAgent.h"
#include "TransactionNotifiers.h"
#include "StatusLine.h"
}

TimerCallback RetransmitTimerAction;
TimerCallback TimeOutTimerAction; 

struct TransactionUserNotifiers MockUser = {
    .onEvent = NULL,
};

int SendOut100TryingMock(char *message)
{
    struct Message *m = CreateMessage();
    ParseMessage(message, m);
    struct StatusLine *sl = MessageGetStatusLine(m);
    
    CHECK_EQUAL(100, StatusLineGetStatusCode(sl));
    STRCMP_EQUAL("Trying", StatusLineGetReasonPhrase(sl));
    DestoryMessage(&m);

    return mock().actualCall(SEND_OUT_MESSAGE_MOCK).returnIntValue();    
}

struct MessageTransporter MockTransporterFor100Trying = {
    SendOut100TryingMock,
    ReceiveInMessageMock,
};

int SendOut180RingingMock(char *message)
{

    struct Message *m = CreateMessage();
    ParseMessage(message, m);
    struct StatusLine *sl = MessageGetStatusLine(m);

    CHECK_EQUAL(180, StatusLineGetStatusCode(sl));
    STRCMP_EQUAL("Ringing", StatusLineGetReasonPhrase(sl));
    DestoryMessage(&m);

    return mock().actualCall(SEND_OUT_MESSAGE_MOCK).returnIntValue();    
}

struct MessageTransporter MockTransporterFor180Ringing = {
    SendOut180RingingMock,
    ReceiveInMessageMock,
};

int SendOut301Mock(char *message)
{

    struct Message *m = CreateMessage();
    ParseMessage(message, m);
    struct StatusLine *sl = MessageGetStatusLine(m);

    CHECK_EQUAL(STATUS_CODE_MOVED_PERMANENTLY, StatusLineGetStatusCode(sl));
    STRCMP_EQUAL(REASON_PHRASE_MOVED_PERMANENTLY, StatusLineGetReasonPhrase(sl));
    DestoryMessage(&m);

    return mock().actualCall(SEND_OUT_MESSAGE_MOCK).returnIntValue();    
}

struct MessageTransporter MockTransporterFor301 = {
    SendOut301Mock,
    ReceiveInMessageMock,
};

int SendOut200OKMock(char *message)
{

    struct Message *m = CreateMessage();
    ParseMessage(message, m);
    struct StatusLine *sl = MessageGetStatusLine(m);

    CHECK_EQUAL(200, StatusLineGetStatusCode(sl));
    STRCMP_EQUAL("OK", StatusLineGetReasonPhrase(sl));
    DestoryMessage(&m);
    return mock().actualCall(SEND_OUT_MESSAGE_MOCK).returnIntValue();    
}

struct MessageTransporter MockTransporterFor200OK = {
    SendOut200OKMock,
    ReceiveInMessageMock,
};

static struct Timer *AddTimerMock(void *data, int interval, TimerCallback action)
{
    if (interval == 64*T1) {
        TimeOutTimerAction = action;
    } else {
        RetransmitTimerAction = action;
    }
    mock().actualCall("AddTimerMock").withParameter("interval", interval);
    return NULL;
}

TEST_GROUP(ServerInviteTransactionTestGroup)
{
    void setup(){
        UT_PTR_SET(ReceiveMessageCallback, MessageReceived);
        UT_PTR_SET(Transporter, &MockTransporter);
    }

    void teardown() {
        EmptyTransactionManager();

        mock().checkExpectations();
        mock().clear();
    }
    
    void CheckOnlyOneTransactionMatched()
    {
        struct Transaction *t = NULL;
        t = GetTransaction((char *)"z9hG4bK27dc30b4",(char *)"INVITE");
        
        CHECK_EQUAL(1, CountTransaction());
        CHECK_TRUE(t != NULL);
        CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));
    }

    void CheckNoTransaction()
    {
        struct Transaction *t = NULL;
        t = GetTransaction((char *)"z9hG4bK27dc30b4",(char *)"INVITE");
        CHECK_EQUAL(0, t);
    }

    struct Transaction *PrepareProceedingState()
    {
        struct TransactionUserNotifiers *user = &MockUser;
        struct Message *request = CreateMessage();

        mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
        ParseMessage((char *)INCOMMING_INVITE_MESSAGE, request);
        struct Transaction *t = AddServerTransaction(request, user);

        return t;
    }

    struct Transaction *PrepareCompletedState()
    {
        struct Transaction *t = PrepareProceedingState();

        UT_PTR_SET(AddTimer, AddTimerMock);
        mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
        mock().expectOneCall("AddTimerMock").withIntParameter("interval", T1);
        mock().expectOneCall("AddTimerMock").withIntParameter("interval", 64*T1);

        UT_PTR_SET(Transporter, &MockTransporterFor301);    
        ResponseWith301(t);
        CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t)); 

        return t;
    }

};

//Create transaction tests.
TEST(ServerInviteTransactionTestGroup, CreateTransactionTest)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};

    UT_PTR_SET(Transporter, &MockTransporterFor100Trying);

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
    ReceiveInMessage(stringReceived); 
    CheckOnlyOneTransactionMatched();
}

TEST(ServerInviteTransactionTestGroup, CreateTransactionTransportErrorTest)
{
    struct TransactionUserNotifiers *user = &MockUser;
    struct Message *request = CreateMessage();

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).andReturnValue(-1);
    ParseMessage((char *)INCOMMING_INVITE_MESSAGE, request);
    AddServerTransaction(request, user);

    CheckNoTransaction();
}

//Proceeding state tests.
TEST(ServerInviteTransactionTestGroup, ProceedingStateReceiveInviteTest)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};
    PrepareProceedingState();

    CheckOnlyOneTransactionMatched();

    //Receive retransmited INVITE message.
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
    ReceiveInMessage(stringReceived); 
    CheckOnlyOneTransactionMatched();
    
    //Receive retransmited INVITE message.
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
    ReceiveInMessage(stringReceived); 
    CheckOnlyOneTransactionMatched();
}

TEST(ServerInviteTransactionTestGroup, ProceedingStateSend180FromTuTest)
{
    struct Transaction *t = PrepareProceedingState();
    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
    UT_PTR_SET(Transporter, &MockTransporterFor180Ringing);
    ResponseWith180Ringing(t);

    CHECK_EQUAL(1, CountTransaction());
    CHECK_TRUE(t != NULL);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));
}

TEST(ServerInviteTransactionTestGroup, ProceedingStateSend180ErrorTest)
{
    struct Transaction *t = PrepareProceedingState();
    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).andReturnValue(-1);
    UT_PTR_SET(Transporter, &MockTransporterFor180Ringing);
    ResponseWith180Ringing(t);

    CheckNoTransaction();
}

TEST(ServerInviteTransactionTestGroup, ProceedingStateSend200OKFromTuTest)
{
    struct Transaction *t = PrepareProceedingState();

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
    UT_PTR_SET(Transporter, &MockTransporterFor200OK);    
    ResponseWith200OK(t);
    
    CheckNoTransaction();
}

TEST(ServerInviteTransactionTestGroup, ProceedingStateSend200OKSendErrorTest)
{
    struct Transaction *t = PrepareProceedingState();

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).andReturnValue(-1);
    UT_PTR_SET(Transporter, &MockTransporterFor200OK);    
    ResponseWith200OK(t);
    
    CheckNoTransaction();
}

TEST(ServerInviteTransactionTestGroup, ProceedingStateSend301FromTuTest)
{
    struct Transaction *t = PrepareProceedingState();

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
    UT_PTR_SET(Transporter, &MockTransporterFor301);    
    ResponseWith301(t);

    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t)); 
}

TEST(ServerInviteTransactionTestGroup, ProceedingStateSend301FromTuAddTimerTest)
{
    struct Transaction *t = PrepareProceedingState();

    mock().expectOneCall("AddTimerMock").withIntParameter("interval", T1);
    mock().expectOneCall("AddTimerMock").withIntParameter("interval", 64*T1);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).andReturnValue(0);    
    UT_PTR_SET(AddTimer, AddTimerMock);

    ResponseWith301(t);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t)); 
}

TEST(ServerInviteTransactionTestGroup, ProceedingState301TransportErrorTest)
{
    struct Transaction *t = PrepareProceedingState();

    UT_PTR_SET(Transporter, &MockTransporterFor301);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).andReturnValue(-1);    
    ResponseWith301(t);
 
    CheckNoTransaction();
}

//Completed State tests
TEST(ServerInviteTransactionTestGroup, CompletedReceiceInviteStateTest)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};
    struct Transaction *t = PrepareCompletedState();

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
    ReceiveInMessage(stringReceived);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t)); 

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
    ReceiveInMessage(stringReceived);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t)); 

}

TEST(ServerInviteTransactionTestGroup, CompletedStateSendResponseErrorTest)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};
    PrepareCompletedState();

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).andReturnValue(-1);
    ReceiveInMessage(stringReceived);

    CheckNoTransaction(); 
}

TEST(ServerInviteTransactionTestGroup, CompletedStateRetransmitTest)
{
    struct Transaction *t = PrepareCompletedState();

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);    
    RetransmitTimerAction(t);

    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t)); 
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
    struct Transaction *t = PrepareCompletedState();    
    ReceiveAckRequest(t);    
    
    CHECK_EQUAL(TRANSACTION_STATE_CONFIRMED, TransactionGetState(t));
}
