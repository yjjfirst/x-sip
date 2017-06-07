#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TestingMessages.h"
#include "Mock.h"

extern "C" {
#include <stdio.h>

#include "UserAgentManager.h"
#include "Transaction.h"
#include "UserAgent.h"
#include "Dialog.h"
#include "DialogId.h"
#include "MessageBuilder.h"
#include "Messages.h"
#include "TransactionManager.h"
#include "Transporter.h"
#include "RequestLine.h"
#include "URI.h"
#include "Header.h"
#include "ContactHeader.h"
#include "CallIdHeader.h"
#include "Session.h"
#include "AccountManager.h"
#include "ViaHeader.h"
#include "CSeqHeader.h"
#include "DialogManager.h"
#include "CallEvents.h"
#include "Accounts.h"
}

int SendAckMessageMock(char *message, char *destaddr, int destport, int fd)
{
    MESSAGE *m = CreateMessage();
    char *remoteTag = NULL;

    ParseMessage(message, m);
    remoteTag = MessageGetToTag(m);    
    mock().actualCall(SEND_OUT_MESSAGE_MOCK).withParameter("RemoteTag", remoteTag);

    CHECK_EQUAL(SIP_METHOD_ACK, RequestLineGetMethod(MessageGetRequestLine(m)));
    DestroyMessage(&m);

    return 0;
}

static struct MessageTransporter MockTransporterForAck = {
    SendAckMessageMock,
    ReceiveMessageMock,
    NULL,
    SipMessageCallback
};

static struct Session *CreateSessionMock()
{
    return NULL;
}

unsigned int CSeqGenerateSeqMock()
{
    return 100;
}

struct Transaction *AddTransactionMock(MESSAGE *message, struct TransactionUser *user, int type)
{
    mock().actualCall("AddTransaction").withParameter("message", message).
        withParameter("type", type); 

    return NULL;
}

MESSAGE *invite_message;
MESSAGE *GetTransactionRequestMock(struct Transaction *t)
{
    invite_message = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite_message);

    return invite_message;
}

TEST_GROUP(UACDialogTestGroup)
{
    struct UserAgent *ua;
    struct Dialog *dialog;
    MESSAGE *invite;
    void setup()
    {
        UT_PTR_SET(SipTransporter, &MockTransporter);
        UT_PTR_SET(CreateSession, CreateSessionMock);
        UT_PTR_SET(GenerateBranch, GenerateBranchMock);
        UT_PTR_SET(CSeqGenerateSeq, CSeqGenerateSeqMock);

        AccountInit();
        ua = CreateUserAgent(0);
        dialog = AddDialog(NULL_DIALOG_ID, ua);
        invite = DialogBuildInvite(dialog, (char *)"88002");
    }

    void teardown()
    {
        ClearAccountManager();
        ClearTransactionManager();
        DestroyUserAgent(&ua);
        
        mock().checkExpectations();
        mock().clear();
    }
};

TEST(UACDialogTestGroup, DialogCreateTest)
{
    CHECK_EQUAL(DIALOG_STATE_NON_EXIST, GetDialogState(dialog));
    DestroyMessage(&invite);
}

TEST(UACDialogTestGroup, SetLocalTagTest)
{
    DialogSetLocalTag(dialog, "testtag123456");

    STRCMP_EQUAL(DialogGetLocalTag(dialog), "testtag123456");
    DestroyMessage(&invite);
}

TEST(UACDialogTestGroup, AckSendAfterInviteSuccessedTest)
{
    UT_PTR_SET(AddTransaction, AddTransactionMock);
    UT_PTR_SET(SipTransporter, &MockTransporterForAck);
    UT_PTR_SET(GetTransactionRequest, GetTransactionRequestMock);
    
    mock().expectOneCall("AddTransaction").withParameter("message", invite).
        withParameter("type", TRANSACTION_TYPE_CLIENT_INVITE);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("RemoteTag", "as6151ad25");

    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_CLIENT_INVITE);

    MESSAGE *ok = CreateMessage();
    ParseMessage(OK_MESSAGE_RECEIVED, ok);

    DialogReceiveOk(dialog, ok);

    DestroyMessage(&invite);
    DestroyMessage(&invite_message);
    DestroyMessage(&ok);
}

TEST(UACDialogTestGroup, AddTransactionTest)
{
    UT_PTR_SET(AddTransaction, AddTransactionMock);

    mock().expectOneCall("AddTransaction").withParameter("message", invite).
        withParameter("type", TRANSACTION_TYPE_CLIENT_INVITE);

    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_CLIENT_INVITE);

    DestroyMessage(&invite);
}

TEST(UACDialogTestGroup, UACDialogIdTest)
{
    char okString[MAX_MESSAGE_LENGTH] = {0};
    MESSAGE *originInvite = DialogBuildInvite(dialog, (char *)"88002");
    MESSAGE *localOk = BuildResponse(originInvite, STATUS_CODE_OK);

    Message2String(okString, localOk);
    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_INVITE));
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(okString);    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_ACK));

    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_CLIENT_INVITE);
    ReceiveMessage();

    STRCMP_EQUAL(MessageGetFromTag(originInvite), DialogGetLocalTag(dialog));     
    STRCMP_EQUAL(MessageGetCallId(originInvite), DialogGetCallId(dialog));
    STRCMP_EQUAL(MessageGetToTag(localOk), DialogGetRemoteTag(dialog));    

    DestroyMessage(&originInvite);
    DestroyMessage(&localOk);
}

TEST(UACDialogTestGroup, UACDialogLocalSeqNumberTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_INVITE));
    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_CLIENT_INVITE);

    CHECK_EQUAL(MessageGetCSeqNumber(invite), GetLocalSeqNumber(dialog));
}

TEST(UACDialogTestGroup, UACDialogRemoteSeqNumberTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_INVITE));
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(OK_MESSAGE_RECEIVED);    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_ACK));
   
    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_CLIENT_INVITE);
    ReceiveMessage();

    CHECK_EQUAL(EMPTY_DIALOG_SEQNUMBER, GetRemoteSeqNumber(dialog));
}

TEST(UACDialogTestGroup, UACDialogRemoteTargetTest)
{
    MESSAGE *ok = CreateMessage();
  
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_INVITE));
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(OK_MESSAGE_RECEIVED);    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_ACK));
 
    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_CLIENT_INVITE);
    ReceiveMessage();

    ParseMessage(OK_MESSAGE_RECEIVED, ok);
    CONTACT_HEADER *ch = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_CONTACT, ok);

    CHECK_TRUE(UriMatched(ContactHeaderGetUri(ch), DialogGetRemoteTarget(dialog)));
    DestroyMessage(&ok);
}

TEST(UACDialogTestGroup, UACDialogRemoteUriTest)
{
    
    URI *remoteUri = DialogGetRemoteUri(dialog);
    CONTACT_HEADER *to = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_TO, invite);

    UriMatched(remoteUri, ContactHeaderGetUri(to));
    DestroyMessage(&invite);
}

TEST(UACDialogTestGroup, UACDialogConfirmedTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_INVITE));
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(OK_MESSAGE_RECEIVED);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_ACK));
   
    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_CLIENT_INVITE);
    ReceiveMessage();

    CHECK_EQUAL(DIALOG_STATE_CONFIRMED, GetDialogState(dialog));

}


TEST(UACDialogTestGroup, UACDialogTerminateTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_BYE));
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);

    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_SERVER_INVITE);
    DialogOk(dialog);
    DialogTerminate(dialog);

    CHECK_EQUAL(DIALOG_STATE_TERMINATED, GetDialogState(dialog));
}

