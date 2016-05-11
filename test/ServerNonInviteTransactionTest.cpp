#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TestingMessages.h"

extern "C" {
#include "Messages.h"
#include "TransactionManager.h"
#include "Transaction.h"
#include "ViaHeader.h"
#include "SipMethod.h"
#include "RequestLine.h"
#include "MessageBuilder.h"
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

//Trying state test
TEST(ServerNonInviteTransactionTestGroup, ServerNonInviteTransactionCreateTest)
{
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, TransactionGetState(transaction));
    CHECK_EQUAL(TRANSACTION_TYPE_SERVER_NON_INVITE, TransactionGetType(transaction));
}

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


TEST(ServerNonInviteTransactionTestGroup, TryingStateReceive1xxTest)
{
    struct Message *trying = BuildTryingMessage(request);

    DestoryMessage(&trying);
}
