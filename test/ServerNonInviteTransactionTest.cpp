#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TestingMessages.h"
#include "Mock.h"

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

#define BYE_MESSAGE (char *)"\
BYE sip:88001@192.168.10.62:5060 SIP/2.0\r\n\
CSeq: 1 BYE\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5060;branch=z9hG4bK56fb2ea6-fe10-e611-972d-60eb69bfc4e8;rport;send-by=192.168.10.101\r\n\
User-Agent: Ekiga/4.0.1\r\n\
From: <sip:88002@192.168.10.1>;tag=2eb78b83-fe10-e611-972d-60eb69bfc4e8\r\n\
Call-ID: 32ee1a624979941474ca0eab6d2d2b37@192.168.10.62:5060\r\n\
To: \"88001\" <sip:88001@192.168.10.62>;tag=as5c1841ad\r\n\
Contact: \"Martin\" <sip:88002@192.168.10.1:5060>\r\n\
Content-Length: 0\r\n\
Max-Forwards: 70\r\n"

TimerCallback TimerJAction;

static struct Timer *AddTimerMock(void *data, int interval, TimerCallback action)
{
    TimerJAction = action;
    mock().actualCall("AddTimerMock").withParameter("interval", interval);
    return NULL;
}

TEST_GROUP(ServerNonInviteTransactionTestGroup)
{
    MESSAGE *request;
    struct Transaction *transaction;
    void setup() 
    {
        UT_PTR_SET(SipTransporter, &DummyTransporter);

        request = CreateMessage();
        ParseMessage(BYE_MESSAGE, request);
        transaction = AddTransaction(request, NULL, TRANSACTION_TYPE_SERVER_NON_INVITE);
    }

    void teardown() 
    {
        ClearTransactionManager();
    }
};

TEST(ServerNonInviteTransactionTestGroup, ServerTransactionRequestMatchTest)
{
    MESSAGE *newRequest = CreateMessage();
    ParseMessage(BYE_MESSAGE, newRequest);

    CHECK_TRUE(RequestTransactionMatched(transaction, newRequest));

    DestroyMessage(&newRequest);
}

TEST(ServerNonInviteTransactionTestGroup, ServerTransactonRequestBranchNonMatchTest)
{
    char branch[] = "1234567890";
    MESSAGE *newRequest = CreateMessage();

    ParseMessage(BYE_MESSAGE, newRequest);
    MessageSetViaBranch(newRequest, branch); 
    
    CHECK_FALSE(RequestTransactionMatched(transaction, newRequest));

    DestroyMessage(&newRequest);
}

TEST(ServerNonInviteTransactionTestGroup, ServerTransactionRequestSendbyNonMatchTest)
{
    MESSAGE *newRequest = CreateMessage();
    ParseMessage(BYE_MESSAGE, newRequest);
    MessageAddViaParameter(newRequest, VIA_SENDBY_PARAMETER_NAME, (char *)"192.168.10.111:777");

    CHECK_FALSE(RequestTransactionMatched(transaction, newRequest));
    DestroyMessage(&newRequest);
}

TEST(ServerNonInviteTransactionTestGroup, ServerTransactionRequestMetodNonMatchedTest)
{
    MESSAGE *newRequest = CreateMessage();
    ParseMessage(BYE_MESSAGE, newRequest);
    struct RequestLine *rl = MessageGetRequestLine(newRequest);
    
    RequestLineSetMethod(rl, (char *)SIP_METHOD_NAME_INVITE);
    CHECK_FALSE(RequestTransactionMatched(transaction, newRequest));
    
    DestroyMessage(&newRequest);
}

TEST(ServerNonInviteTransactionTestGroup, ServerTransactionAckReqestMatchedTest)
{
    ClearTransactionManager();

    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    transaction = AddTransaction(invite, NULL, TRANSACTION_TYPE_SERVER_INVITE);
    
    MESSAGE *ack = BuildAckMessageWithinClientTransaction(invite);

    CHECK_TRUE(RequestTransactionMatched(transaction, ack));

    DestroyMessage(&ack);
}

//Match request to transaction test
TEST(ServerNonInviteTransactionTestGroup, ServerNonInviteTransactionCreateWithInviteTest)
{
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    transaction = AddTransaction(invite, NULL, TRANSACTION_TYPE_SERVER_NON_INVITE);
    
    POINTERS_EQUAL(NULL, transaction);
    
    DestroyMessage(&invite);
}

#define ACK_MESSAGE "\
ACK sip:3401@10.6.2.10:5060 SIP/2.0\r\n\
Via: SIP/2.0/UDP  10.6.3.1:5060;branch=z9hG4bKB1C57\r\n\
From: <sip:4105553501@10.6.3.1>;tag=105741C-1D5E\r\n\
To: <sip:3401@10.6.2.10>;tag=16777231\r\n\
Date: Fri, 06 Jan 2006 5:35:13 GMT\r\n\
Call-ID: E937365B-2C0C11D6-802FA93D-4772A3BB@10.6.3.1\r\n\
Max-Forwards: 70\r\n\
CSeq: 101 ACK\r\n\
Content-Length: 0\r\n"

TEST(ServerNonInviteTransactionTestGroup, ServerNonInviteTransactionCreateWithAckTest)
{
    MESSAGE *ack = CreateMessage();
    ParseMessage(ACK_MESSAGE, ack);
    transaction = AddTransaction(ack, NULL, TRANSACTION_TYPE_SERVER_NON_INVITE);
    
    POINTERS_EQUAL(NULL, transaction);
    
    DestroyMessage(&ack);
}

//Trying state test
TEST(ServerNonInviteTransactionTestGroup, ServerNonInviteTransactionCreateTest)
{
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, GetTransactionState(transaction));
    CHECK_EQUAL(TRANSACTION_TYPE_SERVER_NON_INVITE, GetTransactionType(transaction));
}

TEST(ServerNonInviteTransactionTestGroup, TryingStateSendProvisionalTest)
{
    ResponseWith180Ringing(transaction);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, GetTransactionState(transaction));
}

TEST(ServerNonInviteTransactionTestGroup, TryingStateSendNonprovisionalTest)
{
    ResponseWith200OK(transaction);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, GetTransactionState(transaction));
}

//Proceeding state test
TEST(ServerNonInviteTransactionTestGroup, ProceedingStateSendProvisionalTest)
{
    MESSAGE *trying = BuildTryingMessage(NULL, request);
    ResponseWith180Ringing(transaction);

    for (int i = 0; i < 20; i++) {
        ResponseWith180Ringing(transaction);
        CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, GetTransactionState(transaction));
    }
    DestroyMessage(&trying);
}

TEST(ServerNonInviteTransactionTestGroup, ProceedingStateRequestReceivedTest)
{
    MESSAGE *dupRequest = CreateMessage();
    ParseMessage(BYE_MESSAGE, dupRequest);
    ResponseWith180Ringing(transaction);

    for (int i = 0; i < 20; i++ ){
        ReceiveDupRequest(transaction, dupRequest);
        CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, GetTransactionState(transaction));
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
    UT_PTR_SET(SipTransporter, &MockTransporter);
    ResponseWith180Ringing(transaction);

    CheckNoTransaction();

    mock().checkExpectations();
    mock().clear();
}

TEST(ServerNonInviteTransactionTestGroup, ProceedingState200OKSentTest)
{
    ResponseWith180Ringing(transaction);
    ResponseWith200OK(transaction);

    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, GetTransactionState(transaction));
}

void PrepareCompletedState(struct Transaction *transaction)
{
    ResponseWith180Ringing(transaction);
    ResponseWith200OK(transaction);
}
//Completed state test
TEST(ServerNonInviteTransactionTestGroup, CompletedStateRequestReceivedTest)
{
    MESSAGE *dupRequest = CreateMessage();
    ParseMessage(BYE_MESSAGE, dupRequest);
    PrepareCompletedState(transaction);

    for (int i = 0; i < 20; i++ ){
        ReceiveDupRequest(transaction, dupRequest);
        CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, GetTransactionState(transaction));
    }

    DestroyMessage(&dupRequest);

}

TEST(ServerNonInviteTransactionTestGroup, CompletedStateDupRequestTest)
{
    PrepareCompletedState(transaction);

    MESSAGE *dupRequest = CreateMessage();
    ParseMessage(BYE_MESSAGE, dupRequest);

    UT_PTR_SET(SipTransporter, &MockTransporter);

    for (int i = 0; i < 20; i ++) {
        mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("StatusCode", 200);
        ReceiveDupRequest(transaction, dupRequest);
        CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, GetTransactionState(transaction));
        mock().checkExpectations();
    }
    mock().clear();
    DestroyMessage(&dupRequest);
}

TEST(ServerNonInviteTransactionTestGroup, CompletedStateSendErrorTest)
{
    PrepareCompletedState(transaction);

    MESSAGE *dupRequest = CreateMessage();
    ParseMessage(BYE_MESSAGE, dupRequest);

    UT_PTR_SET(SipTransporter, &MockTransporter);
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
