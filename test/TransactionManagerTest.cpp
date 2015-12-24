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
#include "UserAgent.h"
#include "Dialog.h"
}

enum Response {
    OK200,
    RINGING180,
};

static enum Response Response;

static int ReceiveMessageMock(char *message)
{
    if (Response == RINGING180)
        strcpy(message, BINDING_TRYING_MESSAGE);
    else
        strcpy(message, ADD_BINDING_MESSAGE);
    return 0;
}

static int SendMessageMock(char *message)
{
    return 0;
}

TEST_GROUP(TransactionManager)
{
    void setup() {
        RemoveMessageTransporter((char *)"TRANS");
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

    DestoryTransactionManager();
}

TEST(TransactionManager, Signleton)
{
    struct TransactionManager *m1 = GetTransactionManager();
    struct TransactionManager *m2 = GetTransactionManager();

    CHECK_EQUAL(m1, m2);

    DestoryTransactionManager();
}

TEST(TransactionManager, NewTransaction)
{
    struct UserAgent *ua = CreateUserAgent();
    struct Dialog *dialog = CreateDialog(NULL, ua);
    struct Message *message = BuildBindingMessage(dialog);
    struct Transaction *transaction;

    transaction = AddTransaction(message, NULL);
    CHECK_EQUAL(1, CountTransaction());

    message = BuildBindingMessage(dialog);
    transaction = AddTransaction(message, NULL);
    CHECK_EQUAL(2, CountTransaction());

    message = BuildBindingMessage(dialog);
    transaction = AddTransaction(message, NULL);
    CHECK_EQUAL(3, CountTransaction());


    CHECK_FALSE(0 == transaction)
    DestoryTransactionManager();
    DestoryUserAgent(&ua);
}

TEST(TransactionManager, MatchResponse)
{
    struct UserAgent *ua = CreateUserAgent();
    struct Dialog *dialog = CreateDialog(NULL, ua);
    struct Message *message = BuildBindingMessage(dialog);
    char string[MAX_MESSAGE_LENGTH] = {0};
    
    AddTransaction(message, NULL);
    CHECK_TRUE(ReceiveMessage(string));

    DestoryTransactionManager();
    DestoryUserAgent(&ua);
}

TEST(TransactionManager, BranchNonMatchTest)
{
    char string[MAX_MESSAGE_LENGTH] = {0};
    struct UserAgent *ua = CreateUserAgent();
    struct Dialog *dialog = CreateDialog(NULL, ua);
    struct Message *message = BuildBindingMessage(dialog);
    struct Transaction *t = AddTransaction(message, NULL);
    enum TransactionState s;

    MessageAddViaParameter(message, (char *)"branch", (char *)"z9hG4bK1491280924");

    ReceiveMessage(string);
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);

    DestoryUserAgent(&ua);
    DestoryTransactionManager();
}

TEST(TransactionManager, GetTransactionByTest)
{
    DestoryTransactionManager();

    struct UserAgent *ua = CreateUserAgent();
    struct Dialog *dialog = CreateDialog(NULL, ua);
    struct Message *message = BuildBindingMessage(dialog);
    struct Transaction *t = AddTransaction(message, NULL);
    char seqMethod[] = SIP_METHOD_NAME_REGISTER;
    char branch[] = "z9hG4bK1491280923";

    POINTERS_EQUAL(t, GetTransactionBy(branch, seqMethod));

    DestoryUserAgent(&ua);
    DestoryTransactionManager();
}

