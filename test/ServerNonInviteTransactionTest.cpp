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
#include "MessageTransport.h"
}

TEST_GROUP(ServerNonInviteTransactionTestGroup)
{
    struct Message *request;
    struct Transaction *transaction;
    void setup() 
    {
        request = CreateMessage();
        ParseMessage(BYE_MESSAGE, request);
        transaction = AddServerNonInviteTransaction(request, NULL);

    }

    void teardown() 
    {
        EmptyTransactionManager();
    }
};

TEST(ServerNonInviteTransactionTestGroup, ServerTransactionRequestMatchTest)
{
    struct Message *newRequest = CreateMessage();
    ParseMessage(BYE_MESSAGE, newRequest);

    CHECK_TRUE(IfRequestMatchTransaction(transaction, newRequest));

    DestoryMessage(&newRequest);
}

TEST(ServerNonInviteTransactionTestGroup, ServerTransactonRequestBranchNonMatchTest)
{
    char branch[] = "1234567890";
    struct Message *newRequest = CreateMessage();

    ParseMessage(BYE_MESSAGE, newRequest);
    MessageSetViaBranch(newRequest, branch); 
    
    CHECK_FALSE(IfRequestMatchTransaction(transaction, newRequest));

    DestoryMessage(&newRequest);
}

TEST(ServerNonInviteTransactionTestGroup, ServerTransactionRequestSendbyNonMatchTest)
{
    struct Message *newRequest = CreateMessage();
    ParseMessage(BYE_MESSAGE, newRequest);
    MessageAddViaParameter(newRequest, VIA_SENDBY_PARAMETER_NAME, (char *)"192.168.10.111:777");

    CHECK_FALSE(IfRequestMatchTransaction(transaction, newRequest));
    DestoryMessage(&newRequest);
}

TEST(ServerNonInviteTransactionTestGroup, ServerTransactionRequestMetodNonMatchedTest)
{
    struct Message *newRequest = CreateMessage();
    ParseMessage(BYE_MESSAGE, newRequest);
    struct RequestLine *rl = MessageGetRequestLine(newRequest);
    
    RequestLineSetMethod(rl, (char *)SIP_METHOD_NAME_INVITE);
    CHECK_FALSE(IfRequestMatchTransaction(transaction, newRequest));
    
    DestoryMessage(&newRequest);
}

TEST(ServerNonInviteTransactionTestGroup, ServerTransactionAckReqestMatchedTest)
{
    EmptyTransactionManager();

    struct Message *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    transaction = AddServerInviteTransaction(invite, NULL);
    
    struct Message *ack = BuildAckMessageWithinClientTransaction(invite);

    CHECK_TRUE(IfRequestMatchTransaction(transaction, ack));

    DestoryMessage(&ack);
}

//Match request to transaction test
TEST(ServerNonInviteTransactionTestGroup, ServerNonInviteTransactionCreateWithInviteTest)
{
    struct Message *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    transaction = AddServerNonInviteTransaction(invite, NULL);
    
    POINTERS_EQUAL(NULL, transaction);
    
    DestoryMessage(&invite);
}

TEST(ServerNonInviteTransactionTestGroup, ServerNonInviteTransactionCreateWithAckTest)
{
    struct Message *ack = CreateMessage();
    ParseMessage(ACK_MESSAGE, ack);
    transaction = AddServerNonInviteTransaction(ack, NULL);
    
    POINTERS_EQUAL(NULL, transaction);
    
    DestoryMessage(&ack);
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
    DestoryMessage(&trying);
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

    DestoryMessage(&dupRequest);
}

TEST(ServerNonInviteTransactionTestGroup, ProceedingStateSendErrorTest)
{
    struct Transaction *t = NULL;
    ResponseWith180Ringing(transaction);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("StatusCode", 180).andReturnValue(-1);
    UT_PTR_SET(Transporter, &MockTransporter);
    ResponseWith180Ringing(transaction);
    t = GetTransaction((char *)"z9hG4bK56fb2ea6-fe10-e611-972d-60eb69bfc4e8", (char *)SIP_METHOD_NAME_BYE);
    POINTERS_EQUAL(NULL, t);

    mock().clear();
}

TEST(ServerNonInviteTransactionTestGroup, ProceedingState200OKSentTest)
{
    ResponseWith180Ringing(transaction);
    ResponseWith200OK(transaction);

    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(transaction));
}

//Completed state test
TEST(ServerNonInviteTransactionTestGroup, CompletedStateRequestReceivedTest)
{
    struct Message *dupRequest = CreateMessage();
    ParseMessage(BYE_MESSAGE, dupRequest);
    ResponseWith180Ringing(transaction);
    ResponseWith200OK(transaction);

    for (int i = 0; i < 20; i++ ){
        ReceiveDupRequest(transaction, dupRequest);
        CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(transaction));
    }

    DestoryMessage(&dupRequest);

}

TEST(ServerNonInviteTransactionTestGroup, CompletedStateSendErrorTest)
{
    FAIL("");
}
