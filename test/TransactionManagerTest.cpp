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

static int ReceiveMessageMock(char *message)
{
    strcpy(message, mock().actualCall("ReceiveMessageMock").returnStringValue());
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
        UT_PTR_SET(ReceiveMessageCallback, MessageReceived);
    }

    void teardown() {
        RemoveMessageTransporter((char *)"TRANS");
        mock().clear();
    }
};

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
    EmptyTransactionManager();
    DestoryUserAgent(&ua);
}

TEST(TransactionManager, MatchResponse)
{
    struct UserAgent *ua = CreateUserAgent();
    struct Dialog *dialog = CreateDialog(NULL, ua);
    struct Message *message = BuildBindingMessage(dialog);
    char string[MAX_MESSAGE_LENGTH] = {0};
    
    mock().expectOneCall("ReceiveMessageMock").andReturnValue(ADD_BINDING_MESSAGE);

    AddTransaction(message, NULL);
    CHECK_TRUE(ReceiveMessage(string));

    EmptyTransactionManager();
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

    mock().expectOneCall("ReceiveMessageMock").andReturnValue(ADD_BINDING_MESSAGE);
    MessageAddViaParameter(message, (char *)"branch", (char *)"z9hG4bK1491280924");

    ReceiveMessage(string);
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);

    DestoryUserAgent(&ua);
    EmptyTransactionManager();
}

TEST(TransactionManager, GetTransactionByTest)
{
    EmptyTransactionManager();

    struct UserAgent *ua = CreateUserAgent();
    struct Dialog *dialog = CreateDialog(NULL, ua);
    struct Message *message = BuildBindingMessage(dialog);
    struct Transaction *t = AddTransaction(message, NULL);
    char seqMethod[] = SIP_METHOD_NAME_REGISTER;
    char branch[] = "z9hG4bK1491280923";

    POINTERS_EQUAL(t, GetTransactionBy(branch, seqMethod));

    DestoryUserAgent(&ua);
    EmptyTransactionManager();
}

