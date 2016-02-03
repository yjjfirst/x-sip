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

    return mock().actualCall("SendOutMessageMock").returnIntValue();    
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

    return mock().actualCall("SendOutMessageMock").returnIntValue();    
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

    return mock().actualCall("SendOutMessageMock").returnIntValue();    
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
    return mock().actualCall("SendOutMessageMock").returnIntValue();    
}

struct MessageTransporter MockTransporterFor200OK = {
    SendOut200OKMock,
    ReceiveInMessageMock,
};

static struct Timer *AddTimerMock(void *data, int interval, TimerCallback action)
{
    if (interval == T1)
        RetransmitTimerAction = action;
    else
        TimeOutTimerAction = action;
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
};

//Create transaction tests.
TEST(ServerInviteTransactionTestGroup, CreateTransactionTest)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};
    struct Transaction *t = NULL;

    UT_PTR_SET(Transporter, &MockTransporterFor100Trying);

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall("SendOutMessageMock");
    ReceiveInMessage(stringReceived); 
    t = GetTransaction((char *)"z9hG4bK27dc30b4",(char *)"INVITE");

    CHECK_EQUAL(1, CountTransaction());
    CHECK_TRUE(t != NULL);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));
}

TEST(ServerInviteTransactionTestGroup, CreateTransactionTransportErrorTest)
{
    struct TransactionUserNotifiers *user = &MockUser;
    struct Message *request = CreateMessage();

    mock().expectOneCall("SendOutMessageMock").andReturnValue(-1);
    ParseMessage((char *)INCOMMING_INVITE_MESSAGE, request);
    struct Transaction *t = AddServerTransaction(request, user);

    t = GetTransaction((char *)"z9hG4bK27dc30b4",(char *)"INVITE");
    CHECK_EQUAL(0, t);
}

//Proceeding state tests.
TEST(ServerInviteTransactionTestGroup, ProceedingStateReceiveInviteTest)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};
    struct Transaction *t = NULL;

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall("SendOutMessageMock");
    ReceiveInMessage(stringReceived); 
    t = GetTransaction((char *)"z9hG4bK27dc30b4",(char *)"INVITE");

    CHECK_EQUAL(1, CountTransaction());
    CHECK_TRUE(t != NULL);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));

    //Receive retransmited INVITE message.
    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall("SendOutMessageMock");
    ReceiveInMessage(stringReceived); 

    t = GetTransaction((char *)"z9hG4bK27dc30b4",(char *)"INVITE");

    CHECK_EQUAL(1, CountTransaction());
    CHECK_TRUE(t != NULL);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));
    
    //Receive retransmited INVITE message.
    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall("SendOutMessageMock");
    ReceiveInMessage(stringReceived); 

    t = GetTransaction((char *)"z9hG4bK27dc30b4",(char *)"INVITE");

    CHECK_EQUAL(1, CountTransaction());
    CHECK_TRUE(t != NULL);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));

}

TEST(ServerInviteTransactionTestGroup, ProceedingStateSend180FromTuTest)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};
    struct Transaction *t = NULL;

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall("SendOutMessageMock");
    ReceiveInMessage(stringReceived); 
    t = GetTransaction((char *)"z9hG4bK27dc30b4",(char *)"INVITE");
    
    mock().expectOneCall("SendOutMessageMock");
    UT_PTR_SET(Transporter, &MockTransporterFor180Ringing);
    ResponseWith180Ringing(t);

    CHECK_EQUAL(1, CountTransaction());
    CHECK_TRUE(t != NULL);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));
}

TEST(ServerInviteTransactionTestGroup, ProceedingStateSend180ErrorTest)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};
    struct Transaction *t = NULL;

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall("SendOutMessageMock");
    ReceiveInMessage(stringReceived); 
    t = GetTransaction((char *)"z9hG4bK27dc30b4",(char *)"INVITE");
    
    mock().expectOneCall("SendOutMessageMock").andReturnValue(-1);
    UT_PTR_SET(Transporter, &MockTransporterFor180Ringing);
    ResponseWith180Ringing(t);

    t = GetTransaction((char *)"z9hG4bK27dc30b4",(char *)"INVITE");
    CHECK_EQUAL(0, t);
}


TEST(ServerInviteTransactionTestGroup, ProceedingStateSend200OKFromTuTest)
{
    struct TransactionUserNotifiers *user = &MockUser;
    struct Message *request = CreateMessage();

    mock().expectOneCall("SendOutMessageMock");
    mock().expectOneCall("SendOutMessageMock");
    ParseMessage((char *)INCOMMING_INVITE_MESSAGE, request);

    struct Transaction *t = AddServerTransaction(request, user);
    UT_PTR_SET(Transporter, &MockTransporterFor200OK);    
    ResponseWith200OK(t);
    
    t = GetTransaction((char *)"z9hG4bK27dc30b4",(char *)"INVITE");
    CHECK_EQUAL(0, t);
}

TEST(ServerInviteTransactionTestGroup, ProceedingStateSend200OKSendErrorTest)
{
    struct TransactionUserNotifiers *user = &MockUser;
    struct Message *request = CreateMessage();

    mock().expectOneCall("SendOutMessageMock");
    mock().expectOneCall("SendOutMessageMock").andReturnValue(-1);
    ParseMessage((char *)INCOMMING_INVITE_MESSAGE, request);

    struct Transaction *t = AddServerTransaction(request, user);
    UT_PTR_SET(Transporter, &MockTransporterFor200OK);    
    ResponseWith200OK(t);
    
    t = GetTransaction((char *)"z9hG4bK27dc30b4",(char *)"INVITE");
    CHECK_EQUAL(0, t);
}

TEST(ServerInviteTransactionTestGroup, ProceedingStateSend301FromTuTest)
{
    struct TransactionUserNotifiers *user = &MockUser;
    struct Message *request = CreateMessage();

    mock().expectOneCall("SendOutMessageMock");
    mock().expectOneCall("SendOutMessageMock");
    ParseMessage((char *)INCOMMING_INVITE_MESSAGE, request);

    struct Transaction *t = AddServerTransaction(request, user);
    UT_PTR_SET(Transporter, &MockTransporterFor301);    
    ResponseWith301(t);

    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t)); 
}

TEST(ServerInviteTransactionTestGroup, ProceedingStateSend301FromTuAddTimerTest)
{
    struct TransactionUserNotifiers *user = &MockUser;
    struct Message *request = CreateMessage();

    mock().expectOneCall("SendOutMessageMock");
    mock().expectOneCall("SendOutMessageMock");    

    ParseMessage((char *)INCOMMING_INVITE_MESSAGE, request);
    struct Transaction *t = AddServerTransaction(request, user);

    mock().expectOneCall("AddTimerMock").withIntParameter("interval", T1);
    mock().expectOneCall("AddTimerMock").withIntParameter("interval", 64*T1);
    UT_PTR_SET(AddTimer, AddTimerMock);
    ResponseWith301(t);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t)); 
}

TEST(ServerInviteTransactionTestGroup, ProceedingState301TransportErrorTest)
{
    struct TransactionUserNotifiers *user = &MockUser;
    struct Message *request = CreateMessage();

    mock().expectOneCall("SendOutMessageMock");
    mock().expectOneCall("SendOutMessageMock").andReturnValue(-1);
    ParseMessage((char *)INCOMMING_INVITE_MESSAGE, request);

    struct Transaction *t = AddServerTransaction(request, user);
    UT_PTR_SET(Transporter, &MockTransporterFor301);    
    ResponseWith301(t);

    t = GetTransaction((char *)"z9hG4bK27dc30b4",(char *)"INVITE");
    CHECK_EQUAL(0, t);
}

//Completed State tests
TEST(ServerInviteTransactionTestGroup, CompletedReceiceInviteStateTest)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};
    struct TransactionUserNotifiers *user = &MockUser;
    struct Message *request = CreateMessage();

    mock().expectOneCall("SendOutMessageMock");
    mock().expectOneCall("SendOutMessageMock");
 
    ParseMessage((char *)INCOMMING_INVITE_MESSAGE, request);
    struct Transaction *t = AddServerTransaction(request, user);

    UT_PTR_SET(Transporter, &MockTransporterFor301);    
    ResponseWith301(t);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t)); 

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall("SendOutMessageMock");
    ReceiveInMessage(stringReceived);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t)); 

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall("SendOutMessageMock");
    ReceiveInMessage(stringReceived);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t)); 

}

TEST(ServerInviteTransactionTestGroup, CompletedStateSendResponseErrorTest)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};
    struct TransactionUserNotifiers *user = &MockUser;
    struct Message *request = CreateMessage();

    mock().expectOneCall("SendOutMessageMock");
    mock().expectOneCall("SendOutMessageMock");
 
    ParseMessage((char *)INCOMMING_INVITE_MESSAGE, request);
    struct Transaction *t = AddServerTransaction(request, user);

    UT_PTR_SET(Transporter, &MockTransporterFor301);    
    ResponseWith301(t);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t)); 

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall("SendOutMessageMock").andReturnValue(-1);
    ReceiveInMessage(stringReceived);

    t = GetTransaction((char *)"z9hG4bK27dc30b4",(char *)"INVITE");
    CHECK_EQUAL(0, t);    
}

TEST(ServerInviteTransactionTestGroup, CompletedStateRetransmitTest)
{
    struct TransactionUserNotifiers *user = &MockUser;
    struct Message *request = CreateMessage();

    mock().disable();
    ParseMessage((char *)INCOMMING_INVITE_MESSAGE, request);
    struct Transaction *t = AddServerTransaction(request, user);

    UT_PTR_SET(AddTimer, AddTimerMock);
    ResponseWith301(t);

    mock().enable();
    mock().expectOneCall("SendOutMessageMock");
    RetransmitTimerAction(t);
}

TEST(ServerInviteTransactionTestGroup, CompletedStateRetransmitErrorTest)
{
    struct TransactionUserNotifiers *user = &MockUser;
    struct Message *request = CreateMessage();

    mock().disable();
    ParseMessage((char *)INCOMMING_INVITE_MESSAGE, request);
    struct Transaction *t = AddServerTransaction(request, user);

    UT_PTR_SET(AddTimer, AddTimerMock);
    ResponseWith301(t);

    mock().enable();
    mock().expectOneCall("SendOutMessageMock").andReturnValue(-1);
    RetransmitTimerAction(t);
    
    t = GetTransaction((char *)"z9hG4bK27dc30b4",(char *)"INVITE");
    CHECK_EQUAL(0, t);    
}

TEST(ServerInviteTransactionTestGroup, CompletedStateTimeoutTimerTest)
{
    struct TransactionUserNotifiers *user = &MockUser;
    struct Message *request = CreateMessage();

    mock().disable();
    ParseMessage((char *)INCOMMING_INVITE_MESSAGE, request);
    struct Transaction *t = AddServerTransaction(request, user);

    UT_PTR_SET(AddTimer, AddTimerMock);
    ResponseWith301(t);

    TimeOutTimerAction(t);
    t = GetTransaction((char *)"z9hG4bK27dc30b4",(char *)"INVITE");
    CHECK_EQUAL(0, t);
}

IGNORE_TEST(ServerInviteTransactionTestGroup, CompletedStateReceiveAckTest)
{
    
}
