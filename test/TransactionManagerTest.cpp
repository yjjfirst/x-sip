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
        UT_PTR_SET(ReceiveMessageCallback, MessageReceived);
        UT_PTR_SET(Transporter, &MockTransporter);

        mock().expectOneCall("SendOutMessageMock");
        ua = CreateUserAgent();
        dialog = CreateDialog(NULL, ua);
        message = BuildBindingMessage(dialog);

    }

    void teardown() {
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

    mock().expectOneCall("SendOutMessageMock");
    message = BuildBindingMessage(dialog);
    transaction = AddTransaction(message, NULL);
    CHECK_EQUAL(2, CountTransaction());

    mock().expectOneCall("SendOutMessageMock");
    message = BuildBindingMessage(dialog);
    transaction = AddTransaction(message, NULL);

    CHECK_EQUAL(3, CountTransaction());
    CHECK_FALSE(0 == transaction)
}

TEST(TransactionManager, MatchResponse)
{
    char string[MAX_MESSAGE_LENGTH] = {0};
    
    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(ADD_BINDING_MESSAGE);
    AddTransaction(message, NULL);
    CHECK_TRUE(ReceiveInMessage(string));
}

TEST(TransactionManager, BranchNonMatchTest)
{
    char string[MAX_MESSAGE_LENGTH] = {0};
    struct Transaction *t = AddTransaction(message, NULL);
    enum TransactionState s;

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(ADD_BINDING_MESSAGE);
    MessageAddViaParameter(message, (char *)"branch", (char *)"z9hG4bK1491280924");

    ReceiveInMessage(string);
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

