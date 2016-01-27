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

int SendOut100TryingMock(char *message)
{
    struct Message *m = CreateMessage();
    ParseMessage(message, m);
    struct StatusLine *sl = MessageGetStatusLine(m);
    
    CHECK_EQUAL(100, StatusLineGetStatusCode(sl));
    STRCMP_EQUAL("Trying", StatusLineGetReasonPhrase(sl));
    mock().actualCall("SendOutMessageMock").returnIntValue();    

    DestoryMessage(&m);
    return 0;
}

struct MessageTransporter MockTransporterFor100Trying = {
    SendOut100TryingMock,
    ReceiveInMessageMock,
};


TEST(ServerInviteTransactionTestGroup, ReceiveInvitedCreateTransactionTest)
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

TEST(ServerInviteTransactionTestGroup, ReceiveRetransmitInviteTest)
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
int SendOut180RingingMock(char *message)
{

    struct Message *m = CreateMessage();
    ParseMessage(message, m);
    struct StatusLine *sl = MessageGetStatusLine(m);

    CHECK_EQUAL(180, StatusLineGetStatusCode(sl));
    STRCMP_EQUAL("Ringing", StatusLineGetReasonPhrase(sl));
    mock().actualCall("SendOutMessageMock").returnIntValue();    

    DestoryMessage(&m);
    return 0;
}

struct MessageTransporter MockTransporterFor180Ringing = {
    SendOut180RingingMock,
    ReceiveInMessageMock,
};


TEST(ServerInviteTransactionTestGroup, 1xxFromTuTest)
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

struct TransactionUserNotifiers MockUser = {
    .onEvent = NULL,
};

TEST(ServerInviteTransactionTestGroup, TransportErrorTest)
{
    struct TransactionUserNotifiers *user = &MockUser;
    struct Message *request = CreateMessage();

    mock().expectOneCall("SendOutMessageMock").andReturnValue(-1);
    ParseMessage((char *)INCOMMING_INVITE_MESSAGE, request);
    struct Transaction *t = AddServerTransaction(request, user);
    
    CHECK_EQUAL(TRANSACTION_STATE_TERMINATED, TransactionGetState(t));
}
