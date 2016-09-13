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
}

TEST_GROUP(TransactionManager)
{
    struct UserAgent *ua;
    struct Dialog *dialog;
    MESSAGE *message;
    
    void setup() {
        UT_PTR_SET(Transporter, &MockTransporter);
        UT_PTR_SET(GenerateBranch, GenerateBranchMock);

        AccountInit();
        mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).
            withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER));
        ua = CreateUserAgent(0);
        dialog = AddNewDialog(NULL_DIALOG_ID, ua);
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

    transaction = AddClientNonInviteTransaction(message, NULL);
    CHECK_EQUAL(1, CountTransaction());

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).
        withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER));
    message = BuildAddBindingMessage(dialog);
    transaction = AddClientNonInviteTransaction(message, NULL);
    CHECK_EQUAL(2, CountTransaction());

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).
        withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER));
    message = BuildAddBindingMessage(dialog);
    transaction = AddClientNonInviteTransaction(message, NULL);

    CHECK_EQUAL(3, CountTransaction());
    CHECK_FALSE(0 == transaction);

    mock().checkExpectations();
}

TEST(TransactionManager, MatchResponse)
{
    
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(ADD_BINDING_OK_MESSAGE);
    AddClientNonInviteTransaction(message, NULL);
    CHECK_TRUE(ReceiveInMessage());
}

TEST(TransactionManager, BranchNonMatchTest)
{
    struct Transaction *t = AddClientNonInviteTransaction(message, NULL);
    enum TransactionState s;

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(ADD_BINDING_OK_MESSAGE);
    MessageAddViaParameter(message, (char *)"branch", (char *)"z9hG4bK1491280924");

    ReceiveInMessage();
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);
}

TEST(TransactionManager, GetTransactionByIdTest)
{
    char seqMethod[] = SIP_METHOD_NAME_REGISTER;
    char branch[64];

    strcpy(branch, MessageGetViaBranch(message));
    struct Transaction *t = AddClientNonInviteTransaction(message, NULL);

    POINTERS_EQUAL(t, GetTransaction(branch, seqMethod));
}

TEST(TransactionManager, ExtractTransactionIdFromMessageTest)
{
    MESSAGE *localMessage = CreateMessage();
    struct TransactionId *tid = CreateTransactionId(); 
    ParseMessage(INCOMMING_INVITE_MESSAGE, localMessage);
    TransactionIdExtract(tid, localMessage);

    STRCMP_EQUAL("z9hG4bK27dc30b4",TransactionIdGetBranch(tid));
    STRCMP_EQUAL("INVITE",TransactionIdGetMethod(tid));
   
    DestroyMessage(&localMessage);
    DestroyTransactionId(&tid);
    DestroyMessage(&message);
}

TEST(TransactionManager, TransactionIdTest)
{
    struct Transaction *t = AddClientNonInviteTransaction(message, NULL);
    struct TransactionId *tid = TransactionGetId(t);

    STRCMP_EQUAL(MessageGetViaBranch(message), TransactionIdGetBranch(tid));
    STRCMP_EQUAL(MessageGetCSeqMethod(message), TransactionIdGetMethod(tid));
}

TEST(TransactionManager, TransactionRemoveTest)
{
    struct Transaction *t = AddClientNonInviteTransaction(message, NULL);
    struct TransactionId *tid = TransactionGetId(t);

    RemoveTransactionById(tid);
    POINTERS_EQUAL(NULL, GetTransaction(TransactionIdGetBranch(tid), TransactionIdGetMethod(tid)));
}
