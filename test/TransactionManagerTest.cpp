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
#include "UserAgentManager.h"
}

TEST_GROUP(TransactionManager)
{
    struct UserAgent *ua;
    struct Dialog *dialog;
    
    void setup() {
        UT_PTR_SET(SipTransporter, &MockTransporter);
        UT_PTR_SET(GenerateBranch, GenerateBranchMock);

        AccountInit();
        ua = AddUa(0);
        dialog = AddDialog(NULL_DIALOG_ID, ua);
    }

    void teardown() {
        ClearAccountManager();
        ClearUaManager();
        ClearTransactionManager();

        mock().checkExpectations();
        mock().clear();
    }
};

TEST(TransactionManager, NewTransaction)
{
    struct Transaction *transaction;
    struct Message *message = BuildAddBindingMessage(dialog, 0, 0);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).
        withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER));
    transaction = AddTransaction(message, NULL, TRANSACTION_TYPE_CLIENT_NON_INVITE);
    CHECK_EQUAL(1, CountTransaction());

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).
        withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER));
    message = BuildAddBindingMessage(dialog, 0, 0);
    transaction = AddTransaction(message, NULL, TRANSACTION_TYPE_CLIENT_NON_INVITE);
    CHECK_EQUAL(2, CountTransaction());

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).
        withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER));
    message = BuildAddBindingMessage(dialog, 0, 0);
    transaction = AddTransaction(message, NULL, TRANSACTION_TYPE_CLIENT_NON_INVITE);

    CHECK_EQUAL(3, CountTransaction());
    CHECK_FALSE(0 == transaction);

    mock().checkExpectations();
}

TEST(TransactionManager, MatchResponseTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).
        withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER));

    struct Message *message = BuildAddBindingMessage(dialog, 0, 0);
        
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(ADD_BINDING_OK_MESSAGE);
    AddTransaction(message, NULL, TRANSACTION_TYPE_CLIENT_NON_INVITE);
    CHECK_TRUE(ReceiveMessage());
}

TEST(TransactionManager, BranchNonMatchTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).
        withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER));

    struct Message *message = BuildAddBindingMessage(dialog, 0, 0);    
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

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).
        withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER));

    struct Message *message = BuildAddBindingMessage(dialog, 0, 0);
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
}

TEST(TransactionManager, TransactionIdTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).
        withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER));

    struct Message *message = BuildAddBindingMessage(dialog, 0, 0);
    struct Transaction *t = AddTransaction(message, NULL, TRANSACTION_TYPE_CLIENT_NON_INVITE);
    struct TransactionId *tid = GetTransactionId(t);

    STRCMP_EQUAL(MessageGetViaBranch(message), TransactionIdGetBranch(tid));
    STRCMP_EQUAL(MessageGetCSeqMethod(message), TransactionIdGetMethod(tid));
}

TEST(TransactionManager, TransactionRemoveTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).
        withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER));
   
    struct Message *message = BuildAddBindingMessage(dialog, 0, 0);        
    struct Transaction *t = AddTransaction(message, NULL, TRANSACTION_TYPE_CLIENT_NON_INVITE);
    struct TransactionId *tid = GetTransactionId(t);

    RemoveTransactionById(tid);
    POINTERS_EQUAL(NULL, GetTransaction(TransactionIdGetBranch(tid), TransactionIdGetMethod(tid)));
}

TEST(TransactionManager, MatchResponseToTransactionTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).
        withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER));

    struct Message *message = BuildAddBindingMessage(dialog, 0, 0);
    struct Transaction *t = AddTransaction(message, NULL, TRANSACTION_TYPE_CLIENT_NON_INVITE);
    struct Message *response = CreateMessage();

    ParseMessage(ADD_BINDING_OK_MESSAGE, response);
    POINTERS_EQUAL(t, MatchResponse(response));

    DestroyMessage(&response);
}


TEST(TransactionManager, MatchRequestToTransactionTest)
{
    UT_PTR_SET(SipTransporter, &DummyTransporter);
    
    struct Message *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite); 

    struct Transaction *t = AddTransaction(invite, NULL, TRANSACTION_TYPE_SERVER_INVITE);
    struct Message *reInvite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, reInvite);    
    POINTERS_EQUAL(t, MatchRequest(reInvite));

    DestroyMessage(&reInvite);
}

TEST(TransactionManager, MatchCancelRequestToTransactionTest)
{
    UT_PTR_SET(SipTransporter, &DummyTransporter);
    
    struct Message *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite); 

    struct Transaction *t = AddTransaction(invite, NULL, TRANSACTION_TYPE_SERVER_INVITE);
    struct Message *cancel = CreateMessage();
    ParseMessage(INCOMMING_CANCEL_MESSAGE, cancel);    
    POINTERS_EQUAL(t, MatchRequest(cancel));

    DestroyMessage(&cancel);
}

TEST(TransactionManager, RemoteCancelTest)
{
    struct Message *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);

    struct Message *cancel = CreateMessage();
    ParseMessage(INCOMMING_CANCEL_MESSAGE, cancel);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    SipMessageInput(invite);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 487);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);
    SipMessageInput(cancel);
}

TEST(TransactionManager, RemoteCancelNotifyCmTest)
{
    UT_PTR_SET(SipTransporter, &DummyTransporter);
   
    struct Message *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);

    struct Message *cancel = CreateMessage();
    ParseMessage(INCOMMING_CANCEL_MESSAGE, cancel);

    SipMessageInput(invite);

    UT_PTR_SET(OnTransactionEvent, OnTransactionEventMock);
    mock().expectOneCall("OnTransactionEvent").withParameter("event", TRANSACTION_EVENT_CANCELED);    
    mock().expectOneCall("OnTransactionEvent").withParameter("event", TRANSACTION_EVENT_NEW);    
    SipMessageInput(cancel);

    DestroyMessage(&cancel);
}
