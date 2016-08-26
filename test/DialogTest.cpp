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
}

static struct Session *CreateSessionMock()
{
    return NULL;
}

TEST_GROUP(DialogTestGroup)
{
    struct UserAgent *ua;
    struct Dialog *dialog;
    struct Message *invite;
    struct Message *ok;
    void setup()
    {
        UT_PTR_SET(Transporter, &MockTransporter);
        UT_PTR_SET(CreateSession, CreateSessionMock);
        UT_PTR_SET(GenerateBranch, GenerateBranchMock);

        AccountInit();
        ua = CreateUserAgent(0);
        dialog = AddNewDialog(NULL_DIALOG_ID, ua);
        invite = BuildInviteMessage(dialog);
        ok = Build200OkMessage(invite);
        
    }

    void teardown()
    {
        ClearAccountManager();
        ClearTransactionManager();
        DestroyUserAgent(&ua);
        DestroyMessage(&ok);
        mock().checkExpectations();
        mock().clear();
    }
};

int SendOutAckMessageMock(char *message)
{
    struct Message *m = CreateMessage();
    char *remoteTag = NULL;

    ParseMessage(message, m);
    remoteTag = MessageGetToTag(m);    
    mock().actualCall(SEND_OUT_MESSAGE_MOCK).withParameter("RemoteTag", remoteTag).returnIntValue();

    CHECK_EQUAL(SIP_METHOD_ACK, RequestLineGetMethod(MessageGetRequestLine(m)));
    DestroyMessage(&m);

    return 0;
}

static struct MessageTransporter MockTransporterForAck = {
    SendOutAckMessageMock,
    ReceiveInMessageMock,
};

TEST(DialogTestGroup, DialogCreateTest)
{
    CHECK_EQUAL(DIALOG_STATE_NON_EXIST, DialogGetState(dialog));
    DestroyMessage(&invite);
}

TEST(DialogTestGroup, AckRequestSendAfterInviteSuccessedTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_INVITE));
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_200OK_MESSAGE);    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("RemoteTag", "as6151ad25");
   
    AddClientInviteTransaction(invite, (struct TransactionUser *)dialog);
    UT_PTR_SET(Transporter, &MockTransporterForAck);

    ReceiveInMessage();
}

TEST(DialogTestGroup, AddTransactionTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_INVITE));
    struct Transaction *transaction = DialogAddClientNonInviteTransaction(dialog, invite);

    POINTERS_EQUAL(transaction, GetTransaction(MessageGetViaBranch(invite), MessageGetCSeqMethod(invite)));
}

TEST(DialogTestGroup, UACDialogIdTest)
{
    char okString[MAX_MESSAGE_LENGTH] = {0};
    struct Message *originInvite = BuildInviteMessage(dialog);
    struct Message *localOk = Build200OkMessage(originInvite);
    
    Message2String(okString, localOk);
    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_INVITE));
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(okString);    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_ACK));

    DialogAddClientInviteTransaction(dialog, invite);
    ReceiveInMessage();
    
    STRCMP_EQUAL(MessageGetFromTag(originInvite), DialogIdGetLocalTag(DialogGetId(dialog)));     
    STRCMP_EQUAL(MessageGetCallId(originInvite), DialogIdGetCallId(DialogGetId(dialog)));
    STRCMP_EQUAL(MessageGetToTag(localOk), DialogIdGetRemoteTag(DialogGetId(dialog)));    

    DestroyMessage(&originInvite);
    DestroyMessage(&localOk);
}

TEST(DialogTestGroup, UACDialogIdDelegateTest)
{
    char okString[MAX_MESSAGE_LENGTH] = {0};
    struct Message *originInvite = BuildInviteMessage(dialog);
    struct Message *localOk = Build200OkMessage(originInvite);
    Message2String(okString, localOk);
    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_INVITE));
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(okString);    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_ACK));

    DialogAddClientInviteTransaction(dialog, invite);
    ReceiveInMessage();

    STRCMP_EQUAL(MessageGetFromTag(originInvite), DialogGetLocalTag(dialog));     
    STRCMP_EQUAL(MessageGetCallId(originInvite), DialogGetCallId(dialog));
    STRCMP_EQUAL(MessageGetToTag(localOk), DialogGetRemoteTag(dialog));    

    DestroyMessage(&originInvite);
    DestroyMessage(&localOk);

}

TEST(DialogTestGroup, UACDialogLocalSeqNumberTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_INVITE));
    DialogAddClientInviteTransaction(dialog, invite);

    CHECK_EQUAL(MessageGetCSeqNumber(invite), DialogGetLocalSeqNumber(dialog));
}

TEST(DialogTestGroup, UACDialogRemoteSeqNumberTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_INVITE));
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_200OK_MESSAGE);    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_ACK));
   
    DialogAddClientInviteTransaction(dialog, invite);
    ReceiveInMessage();

    CHECK_EQUAL(EMPTY_DIALOG_SEQNUMBER, DialogGetRemoteSeqNumber(dialog));
}

TEST(DialogTestGroup, UACDialogRemoteTargetTest)
{
    struct Message *ok = CreateMessage();
  
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_INVITE));
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_200OK_MESSAGE);    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_ACK));
 
    DialogAddClientInviteTransaction(dialog, invite);
    ReceiveInMessage();

    ParseMessage(INVITE_200OK_MESSAGE, ok);
    struct ContactHeader *ch = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_CONTACT, ok);

    CHECK_TRUE(UriMatched(ContactHeaderGetUri(ch), DialogGetRemoteTarget(dialog)));
    DestroyMessage(&ok);
}

TEST(DialogTestGroup, UACDialogConfirmedTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_INVITE));
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_200OK_MESSAGE);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_ACK));
   
    DialogAddClientInviteTransaction(dialog, invite);
    ReceiveInMessage();

    CHECK_EQUAL(DIALOG_STATE_CONFIRMED, DialogGetState(dialog));

}

TEST(DialogTestGroup, UACDialogTerminateTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_BYE));
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);

    DialogAddServerInviteTransaction(dialog, invite);
    DialogSend200OKResponse(dialog);
    DialogTerminate(dialog);

    CHECK_EQUAL(DIALOG_STATE_TERMINATED, DialogGetState(dialog));

    DestroyMessage(&ok);
}

TEST(DialogTestGroup, UASDialogIdTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);
    struct ContactHeader *from = ContactHeaderDup((struct ContactHeader *)MessageGetHeader(HEADER_NAME_FROM, invite));
    struct CallIdHeader *id = CallIdHeaderDup((struct CallIdHeader *)MessageGetHeader(HEADER_NAME_CALLID, invite));

    DialogAddServerInviteTransaction(dialog, invite);
    DialogSend200OKResponse(dialog);

    STRCMP_EQUAL(MessageGetToTag(ok), DialogIdGetLocalTag(DialogGetId(dialog)));
    STRCMP_EQUAL(ContactHeaderGetParameter(from, HEADER_PARAMETER_NAME_TAG), DialogIdGetRemoteTag(DialogGetId(dialog)));
    STRCMP_EQUAL(CallIdHeaderGetId(id), DialogIdGetCallId(DialogGetId(dialog)));

    DestroyContactHeader((struct Header *)from);
    DestroyCallIdHeader((struct Header *)id);
}

TEST(DialogTestGroup, UASDialogConfirmedTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);

    DialogAddServerInviteTransaction(dialog, invite);
    DialogSend200OKResponse(dialog);

    CHECK_EQUAL(DIALOG_STATE_CONFIRMED, DialogGetState(dialog));
    DestroyMessage(&ok);
}

TEST(DialogTestGroup, UASDialogRemoteSeqNumberTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);

    unsigned int seq = MessageGetCSeqNumber(invite);
    DialogAddServerInviteTransaction(dialog, invite);
    DialogSend200OKResponse(dialog);

    CHECK_EQUAL(seq, DialogGetRemoteSeqNumber(dialog));
    DestroyMessage(&ok);
}

TEST(DialogTestGroup, UASDialogLocalSeqNumberTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);

    DialogAddServerInviteTransaction(dialog, invite);
    DialogSend200OKResponse(dialog);

    CHECK_EQUAL(EMPTY_DIALOG_SEQNUMBER, DialogGetLocalSeqNumber(dialog));
    DestroyMessage(&ok);
}

TEST(DialogTestGroup, UASDialogRemoteTargetTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);

    struct ContactHeader *c = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_CONTACT, invite);
    struct URI *uri = UriDup(ContactHeaderGetUri(c));
    DialogAddServerInviteTransaction(dialog, invite);
    DialogSend200OKResponse(dialog);

    CHECK_TRUE(UriMatched(uri, DialogGetRemoteTarget(dialog)));

    DestroyUri(&uri);
    DestroyMessage(&ok);
}

TEST(DialogTestGroup, UASDialogTerminateTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);

    DialogAddServerInviteTransaction(dialog, invite); 
    DialogSend200OKResponse(dialog);

    struct Message *bye = BuildByeMessage(dialog);
    DialogReceiveBye(dialog, bye);
    CHECK_EQUAL(DIALOG_STATE_TERMINATED, DialogGetState(dialog));
}
