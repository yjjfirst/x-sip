#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TestingMessages.h"
#include "TransportMock.h"

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

TEST_GROUP(TransactionManager)
{
    struct UserAgent *ua;
    struct Dialog *dialog;
    struct Message *message;
    
    void setup() {
        AddMessageTransporter((char *)"TRANS", SendMessageMock, ReceiveMessageMock);
        UT_PTR_SET(ReceiveMessageCallback, MessageReceived);

        mock().expectOneCall("SendMessageMock");
        ua = CreateUserAgent();
        dialog = CreateDialog(NULL, ua);
        message = BuildBindingMessage(dialog);

    }

    void teardown() {
        RemoveMessageTransporter((char *)"TRANS");
        mock().clear();

        DestoryUserAgent(&ua);
        EmptyTransactionManager();
    }
};

TEST(TransactionManager, NewTransaction)
{
    struct Transaction *transaction;

    transaction = AddTransaction(message, NULL);
    CHECK_EQUAL(1, CountTransaction());

    mock().expectOneCall("SendMessageMock");
    message = BuildBindingMessage(dialog);
    transaction = AddTransaction(message, NULL);
    CHECK_EQUAL(2, CountTransaction());

    mock().expectOneCall("SendMessageMock");
    message = BuildBindingMessage(dialog);
    transaction = AddTransaction(message, NULL);

    CHECK_EQUAL(3, CountTransaction());
    CHECK_FALSE(0 == transaction)
}

TEST(TransactionManager, MatchResponse)
{
    char string[MAX_MESSAGE_LENGTH] = {0};
    
    mock().expectOneCall("ReceiveMessageMock").andReturnValue(ADD_BINDING_MESSAGE);
    AddTransaction(message, NULL);
    CHECK_TRUE(ReceiveMessage(string));
}

TEST(TransactionManager, BranchNonMatchTest)
{
    char string[MAX_MESSAGE_LENGTH] = {0};
    struct Transaction *t = AddTransaction(message, NULL);
    enum TransactionState s;

    mock().expectOneCall("ReceiveMessageMock").andReturnValue(ADD_BINDING_MESSAGE);
    MessageAddViaParameter(message, (char *)"branch", (char *)"z9hG4bK1491280924");

    ReceiveMessage(string);
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);
}

TEST(TransactionManager, GetTransactionByTest)
{
    char seqMethod[] = SIP_METHOD_NAME_REGISTER;
    char branch[] = "z9hG4bK1491280923";
    struct Transaction *t = AddTransaction(message, NULL);

    POINTERS_EQUAL(t, GetTransactionBy(branch, seqMethod));
}
