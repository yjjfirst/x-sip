#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TestingMessages.h"

extern "C" {
#include <stdio.h>
#include <string.h>
#include "Messages.h"
#include "TransactionManager.h"
#include "MessageBuilder.h"
#include "MessageTransport.h"
#include "Transaction.h"
#include "Method.h"
#include "UserAgent.h"
}

enum Response {
    OK200,
    RINGING180,
};

static enum Response Response;

static int ReceiveMessageMock(char *message)
{
    if (Response == RINGING180)
        strcpy(message, TryingMessage);
    else
        strcpy(message, OKMessage);
    return 0;
}

static int SendMessageMock(char *message)
{
    return 0;
}

TEST_GROUP(TransactionManager)
{
    void setup() {
        AddMessageTransporter((char *)"TRANS", SendMessageMock, ReceiveMessageMock);
        InitReceiveMessageCallback(MessageReceived);
    }

    void teardown() {
        RemoveMessageTransporter((char *)"TRANS");
    }
};

TEST(TransactionManager, Init)
{
    struct TransactionManager *manager = GetTransactionManager();
    CHECK_FALSE(manager == NULL);

    DestoryTransactionManager(&manager);
    CHECK_TRUE(manager == NULL);    
}

TEST(TransactionManager, Signleton)
{
    struct TransactionManager *m1 = GetTransactionManager();
    struct TransactionManager *m2 = GetTransactionManager();

    CHECK_EQUAL(m1, m2);

    DestoryTransactionManager(&m1);
}

TEST(TransactionManager, NewTransaction)
{
    struct UserAgent *ua = CreateUserAgent();
    struct Message *message = BuildRegisterMessage(ua);
    struct TransactionManager *manager = GetTransactionManager();
    struct Transaction *transaction;

    transaction = CreateTransactionExt(message);
    CHECK_EQUAL(1, CountTransaction());

    message = BuildRegisterMessage(ua);
    transaction = CreateTransactionExt(message);
    CHECK_EQUAL(2, CountTransaction());

    message = BuildRegisterMessage(ua);
    transaction = CreateTransactionExt(message);
    CHECK_EQUAL(3, CountTransaction());


    CHECK_FALSE(0 == transaction)
    DestoryTransactionManager(&manager);
    DestoryUserAgent(&ua);
}

TEST(TransactionManager, MatchResponse)
{
    struct UserAgent *ua = CreateUserAgent();
    struct Message *message = BuildRegisterMessage(ua);
    struct TransactionManager *manager = GetTransactionManager();
    char string[MAX_MESSAGE_LENGTH] = {0};
    
    MessageAddViaParameter(message, (char *)"rport", (char *)"");
    MessageAddViaParameter(message, (char *)"branch", (char *)"z9hG4bK1491280923");

    CreateTransactionExt(message);
    CHECK_TRUE(ReceiveMessage(string));

    DestoryTransactionManager(&manager);
    DestoryUserAgent(&ua);
}

TEST(TransactionManager, BranchNonMatchTest)
{
    char string[MAX_MESSAGE_LENGTH] = {0};
    struct UserAgent *ua = CreateUserAgent();
    struct Message *message = BuildRegisterMessage(ua);
    struct TransactionManager *manager = GetTransactionManager();
    struct Transaction *t = CreateTransactionExt(message);
    enum TransactionState s;

    MessageAddViaParameter(message, (char *)"branch", (char *)"z9hG4bK1491280924");

    ReceiveMessage(string);
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);

    DestoryUserAgent(&ua);    
    DestoryTransactionManager(&manager);
}

TEST(TransactionManager, GetTransactionByTest)
{
    struct UserAgent *ua = CreateUserAgent();
    struct Message *message = BuildRegisterMessage(ua);
    struct TransactionManager *manager = GetTransactionManager();
    struct Transaction *t = CreateTransactionExt(message);
    char seqMethod[] = SIP_METHOD_NAME_REGISTER;
    char branch[] = "z9hG4bK1491280923";

    MessageAddViaParameter(message, (char *)"branch", (char *)"z9hG4bK1491280923");

    POINTERS_EQUAL(t, GetTransactionBy(branch, seqMethod));

    DestoryUserAgent(&ua);
    DestoryTransactionManager(&manager);
}

