#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TestingMessages.h"
#include "Mock.h"

extern "C" {
#include <stdio.h>
#include <string.h>

#include "Messages.h"
#include "TransactionManager.h"
#include "MessageBuilder.h"
#include "Transporter.h"
#include "Transaction.h"
#include "TransactionId.h"
#include "UserAgent.h"
#include "Dialog.h"
#include "AccountManager.h"
#include "ViaHeader.h"
#include "DialogManager.h"
}

TEST_GROUP(TransactionManager)
{
    struct UserAgent *ua;
    struct Dialog *dialog;
    MESSAGE *message;
    
    void setup() {
        UT_PTR_SET(SipTransporter, &MockTransporter);
        UT_PTR_SET(GenerateBranch, GenerateBranchMock);

        AccountInit();
        mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).
            withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER));
        ua = CreateUserAgent(0);
        dialog = AddDialog(NULL_DIALOG_ID, ua);
        message = BuildAddBindingMessage(dialog);
    }

    void teardown() {
        ClearAccountManager();
        mock().clear();
        DestroyUserAgent(&ua);
        ClearTransactionManager();
    }
};

TEST(TransactionManager, NewTransaction)
{
    struct Transaction *transaction;

    transaction = AddTransaction(message, NULL, TRANSACTION_TYPE_CLIENT_NON_INVITE);
    CHECK_EQUAL(1, CountTransaction());

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).
        withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER));
    message = BuildAddBindingMessage(dialog);
    transaction = AddTransaction(message, NULL, TRANSACTION_TYPE_CLIENT_NON_INVITE);
    CHECK_EQUAL(2, CountTransaction());

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).
        withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER));
    message = BuildAddBindingMessage(dialog);
    transaction = AddTransaction(message, NULL, TRANSACTION_TYPE_CLIENT_NON_INVITE);

    CHECK_EQUAL(3, CountTransaction());
    CHECK_FALSE(0 == transaction);

    mock().checkExpectations();
}

TEST(TransactionManager, MatchResponse)
{
    
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(ADD_BINDING_OK_MESSAGE);
    AddTransaction(message, NULL, TRANSACTION_TYPE_CLIENT_NON_INVITE);
    CHECK_TRUE(ReceiveMessage());
}

TEST(TransactionManager, BranchNonMatchTest)
{
    struct Transaction *t = AddTransaction(message, NULL, TRANSACTION_TYPE_CLIENT_NON_INVITE);
    enum TransactionState s;

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(ADD_BINDING_OK_MESSAGE);
    MessageAddViaParameter(message, (char *)"branch", (char *)"z9hG4bK1491280924");

    ReceiveMessage();
    s = GetTransactionState(t);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);
}

TEST(TransactionManager, GetTransactionByIdTest)
{
    char seqMethod[] = SIP_METHOD_NAME_REGISTER;
    char branch[64];

    strcpy(branch, MessageGetViaBranch(message));
    struct Transaction *t = AddTransaction(message, NULL, TRANSACTION_TYPE_CLIENT_NON_INVITE);

    POINTERS_EQUAL(t, GetTransaction(branch, seqMethod));
}

TEST(TransactionManager, ExtractTransactionIdFromMessageTest)
{
    MESSAGE *localMessage = CreateMessage();
    struct TransactionId *tid = CreateTransactionId(); 
    ParseMessage(INCOMMING_INVITE_MESSAGE, localMessage);
    ExtractTransactionId(tid, localMessage);

    STRCMP_EQUAL("z9hG4bK27dc30b4",TransactionIdGetBranch(tid));
    STRCMP_EQUAL("INVITE",TransactionIdGetMethod(tid));
   
    DestroyMessage(&localMessage);
    DestroyTransactionId(&tid);
    DestroyMessage(&message);
}

TEST(TransactionManager, TransactionIdTest)
{
    struct Transaction *t = AddTransaction(message, NULL, TRANSACTION_TYPE_CLIENT_NON_INVITE);
    struct TransactionId *tid = GetTransactionId(t);

    STRCMP_EQUAL(MessageGetViaBranch(message), TransactionIdGetBranch(tid));
    STRCMP_EQUAL(MessageGetCSeqMethod(message), TransactionIdGetMethod(tid));
}

TEST(TransactionManager, TransactionRemoveTest)
{
    struct Transaction *t = AddTransaction(message, NULL, TRANSACTION_TYPE_CLIENT_NON_INVITE);
    struct TransactionId *tid = GetTransactionId(t);

    RemoveTransactionById(tid);
    POINTERS_EQUAL(NULL, GetTransaction(TransactionIdGetBranch(tid), TransactionIdGetMethod(tid)));
}
