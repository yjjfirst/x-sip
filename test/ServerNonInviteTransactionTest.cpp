#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TestingMessages.h"
#include "TransportMock.h"

extern "C" {
#include <stdio.h>
#include "Messages.h"
#include "TransactionManager.h"
#include "Transaction.h"
#include "ViaHeader.h"
#include "SipMethod.h"
#include "RequestLine.h"
#include "MessageBuilder.h"
#include "Transporter.h"
}

TimerCallback TimerJAction;

static struct Timer *AddTimerMock(void *data, int interval, TimerCallback action)
{
    TimerJAction = action;
    mock().actualCall("AddTimerMock").withParameter("interval", interval);
    return NULL;
}

TEST_GROUP(ServerNonInviteTransactionTestGroup)
{
    struct Message *request;
    struct Transaction *transaction;
    void setup() 
    {
        UT_PTR_SET(Transporter, &DummyTransporter);

        request = CreateMessage();
        ParseMessage(BYE_MESSAGE, request);
        transaction = AddServerNonInviteTransaction(request, NULL);
    }

    void teardown() 
    {
        ClearTransactionManager();
    }
};

TEST(ServerNonInviteTransactionTestGroup, ServerTransactionRequestMatchTest)
{
    struct Message *newRequest = CreateMessage();
    ParseMessage(BYE_MESSAGE, newRequest);

    CHECK_TRUE(IfRequestMatchTransaction(transaction, newRequest));

    DestroyMessage(&newRequest);
}

TEST(ServerNonInviteTransactionTestGroup, ServerTransactonRequestBranchNonMatchTest)
{
    char branch[] = "1234567890";
    struct Message *newRequest = CreateMessage();

    ParseMessage(BYE_MESSAGE, newRequest);
    MessageSetViaBranch(newRequest, branch); 
    
    CHECK_FALSE(IfRequestMatchTransaction(transaction, newRequest));

    DestroyMessage(&newRequest);
}

TEST(ServerNonInviteTransactionTestGroup, ServerTransactionRequestSendbyNonMatchTest)
{
    struct Message *newRequest = CreateMessage();
    ParseMessage(BYE_MESSAGE, newRequest);
    MessageAddViaParameter(newRequest, VIA_SENDBY_PARAMETER_NAME, (char *)"192.168.10.111:777");

    CHECK_FALSE(IfRequestMatchTransaction(transaction, newRequest));
    DestroyMessage(&newRequest);
}

TEST(ServerNonInviteTransactionTestGroup, ServerTransactionRequestMetodNonMatchedTest)
{
    struct Message *newRequest = CreateMessage();
    ParseMessage(BYE_MESSAGE, newRequest);
    struct RequestLine *rl = MessageGetRequestLine(newRequest);
    
    RequestLineSetMethod(rl, (char *)SIP_METHOD_NAME_INVITE);
    CHECK_FALSE(IfRequestMatchTransaction(transaction, newRequest));
    
    DestroyMessage(&newRequest);
}

TEST(ServerNonInviteTransactionTestGroup, ServerTransactionAckReqestMatchedTest)
{
    ClearTransactionManager();

    struct Message *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    transaction = AddServerInviteTransaction(invite, NULL);
    
    struct Message *ack = BuildAckMessageWithinClientTransaction(invite);

    CHECK_TRUE(IfRequestMatchTransaction(transaction, ack));

    DestroyMessage(&ack);
}

//Match request to transaction test
TEST(ServerNonInviteTransactionTestGroup, ServerNonInviteTransactionCreateWithInviteTest)
{
    struct Message *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    transaction = AddServerNonInviteTransaction(invite, NULL);
    
    POINTERS_EQUAL(NULL, transaction);
    
    DestroyMessage(&invite);
}

TEST(ServerNonInviteTransactionTestGroup, ServerNonInviteTransactionCreateWithAckTest)
{
    struct Message *ack = CreateMessage();
    ParseMessage(ACK_MESSAGE, ack);
    transaction = AddServerNonInviteTransaction(ack, NULL);
    
    POINTERS_EQUAL(NULL, transaction);
    
    DestroyMessage(&ack);
}

//Trying state test
TEST(ServerNonInviteTransactionTestGroup, ServerNonInviteTransactionCreateTest)
{
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, TransactionGetState(transaction));
    CHECK_EQUAL(TRANSACTION_TYPE_SERVER_NON_INVITE, TransactionGetType(transaction));
}

TEST(ServerNonInviteTransactionTestGroup, TryingStateSendProvisionalTest)
{
    ResponseWith180Ringing(transaction);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(transaction));
}

TEST(ServerNonInviteTransactionTestGroup, TryingStateSendNonprovisionalTest)
{
    ResponseWith200OK(transaction);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(transaction));
}

//Proceeding state test
TEST(ServerNonInviteTransactionTestGroup, ProceedingStateSendProvisionalTest)
{
    struct Message *trying = BuildTryingMessage(request);
    ResponseWith180Ringing(transaction);

    for (int i = 0; i < 20; i++) {
        ResponseWith180Ringing(transaction);
        CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(transaction));
    }
    DestroyMessage(&trying);
}

TEST(ServerNonInviteTransactionTestGroup, ProceedingStateRequestReceivedTest)
{
    struct Message *dupRequest = CreateMessage();
    ParseMessage(BYE_MESSAGE, dupRequest);
    ResponseWith180Ringing(transaction);

    for (int i = 0; i < 20; i++ ){
        ReceiveDupRequest(transaction, dupRequest);
        CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(transaction));
    }

    DestroyMessage(&dupRequest);
}

void CheckNoTransaction()
{
    struct Transaction *t = NULL;
    t = GetTransaction((char *)"z9hG4bK56fb2ea6-fe10-e611-972d-60eb69bfc4e8", (char *)SIP_METHOD_NAME_BYE);
    POINTERS_EQUAL(NULL, t);
}

TEST(ServerNonInviteTransactionTestGroup, ProceedingStateSendErrorTest)
{
    ResponseWith180Ringing(transaction);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("StatusCode", 180).andReturnValue(-1);
    UT_PTR_SET(Transporter, &MockTransporter);
    ResponseWith180Ringing(transaction);

    CheckNoTransaction();

    mock().checkExpectations();
    mock().clear();
}

TEST(ServerNonInviteTransactionTestGroup, ProceedingState200OKSentTest)
{
    ResponseWith180Ringing(transaction);
    ResponseWith200OK(transaction);

    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(transaction));
}

void PrepareCompletedState(struct Transaction *transaction)
{
    ResponseWith180Ringing(transaction);
    ResponseWith200OK(transaction);
}
//Completed state test
TEST(ServerNonInviteTransactionTestGroup, CompletedStateRequestReceivedTest)
{
    struct Message *dupRequest = CreateMessage();
    ParseMessage(BYE_MESSAGE, dupRequest);
    PrepareCompletedState(transaction);

    for (int i = 0; i < 20; i++ ){
        ReceiveDupRequest(transaction, dupRequest);
        CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(transaction));
    }

    DestroyMessage(&dupRequest);

}

TEST(ServerNonInviteTransactionTestGroup, CompletedStateDupRequestTest)
{
    PrepareCompletedState(transaction);

    struct Message *dupRequest = CreateMessage();
    ParseMessage(BYE_MESSAGE, dupRequest);

    UT_PTR_SET(Transporter, &MockTransporter);

    for (int i = 0; i < 20; i ++) {
        mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("StatusCode", 200);
        ReceiveDupRequest(transaction, dupRequest);
        CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(transaction));
        mock().checkExpectations();
    }
    mock().clear();
    DestroyMessage(&dupRequest);
}

TEST(ServerNonInviteTransactionTestGroup, CompletedStateSendErrorTest)
{
    PrepareCompletedState(transaction);

    struct Message *dupRequest = CreateMessage();
    ParseMessage(BYE_MESSAGE, dupRequest);

    UT_PTR_SET(Transporter, &MockTransporter);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("StatusCode", 200).andReturnValue(-1);
    ReceiveDupRequest(transaction, dupRequest);

    CheckNoTransaction();

    mock().checkExpectations();
    mock().clear();
    DestroyMessage(&dupRequest);
}

TEST(ServerNonInviteTransactionTestGroup, ToCompletedStateFromProceedingAddTimerTest)
{
    UT_PTR_SET(AddTimer, AddTimerMock);
    mock().expectOneCall("AddTimerMock").withParameter("interval", WAIT_TIME_FOR_REQUEST_RETRANSMITS);

    PrepareCompletedState(transaction);
    
    mock().checkExpectations();
    mock().clear();
}

TEST(ServerNonInviteTransactionTestGroup, ToCompletedStateFromTryinbgAddTimerTest)
{
    UT_PTR_SET(AddTimer, AddTimerMock);
    mock().expectOneCall("AddTimerMock").withParameter("interval", WAIT_TIME_FOR_REQUEST_RETRANSMITS);

    ResponseWith200OK(transaction);
    
    mock().checkExpectations();
    mock().clear();
}

TEST(ServerNonInviteTransactionTestGroup, CompletedStateTimerJTest)
{
    UT_PTR_SET(AddTimer, AddTimerMock);
    mock().expectOneCall("AddTimerMock").withParameter("interval", WAIT_TIME_FOR_REQUEST_RETRANSMITS);

    PrepareCompletedState(transaction);
    TimerJAction(transaction);

    CheckNoTransaction();

    mock().checkExpectations();
    mock().clear();
}
