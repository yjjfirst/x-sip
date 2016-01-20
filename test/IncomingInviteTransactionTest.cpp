#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TransportMock.h"
#include "TestingMessages.h"

extern "C" {
#include <stdio.h>
#include "Messages.h"
#include "MessageTransport.h"
#include "TransactionManager.h"
#include "Transaction.h"
#include "UserAgent.h"
}

TEST_GROUP(IncomingInviteTransactionTestGroup)
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

TEST(IncomingInviteTransactionTestGroup, ReceiveInvitedCreateTransactionTest)
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
}

TEST(IncomingInviteTransactionTestGroup, Send100TryingTest)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall("SendOutMessageMock");
    ReceiveInMessage(stringReceived);    
}

