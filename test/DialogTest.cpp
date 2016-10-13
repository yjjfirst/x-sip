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
    mock().actualCall(SEND_OUT_MESSAGE_MOCK).withParameter("RemoteTag", remoteTag).returnIntValue();

    CHECK_EQUAL(SIP_METHOD_ACK, RequestLineGetMethod(MessageGetRequestLine(m)));
    DestroyMessage(&m);

    return 0;
}

static struct MessageTransporter MockTransporterForAck = {
    SendAckMessageMock,
    ReceiveMessageMock,
    NULL,
    SipMessageHandle

};

static struct Session *CreateSessionMock()
{
    return NULL;
}

unsigned int CSeqGenerateSeqMock()
{
    return 100;
}

TEST_GROUP(DialogTestGroup)
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
        invite = BuildInviteMessage(dialog, (char *)"88002");
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

TEST(DialogTestGroup, DialogCreateTest)
{
    CHECK_EQUAL(DIALOG_STATE_NON_EXIST, DialogGetState(dialog));
    DestroyMessage(&invite);
}

TEST(DialogTestGroup, SetLocalTagTest)
{
    DialogSetLocalTag(dialog, "testtag123456");

    STRCMP_EQUAL(DialogGetLocalTag(dialog), "testtag123456");
    DestroyMessage(&invite);
}

TEST(DialogTestGroup, AckRequestSendAfterInviteSuccessedTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_INVITE));
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("RemoteTag", "as6151ad25");

    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_CLIENT_INVITE);
    UT_PTR_SET(SipTransporter, &MockTransporterForAck);

    MESSAGE *ok = CreateMessage();
    ParseMessage(OK_MESSAGE_RECEIVED, ok);
    DialogReceiveOk(dialog, ok);

    DestroyMessage(&ok);
}

TEST(DialogTestGroup, AddTransactionTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_INVITE));
    struct Transaction *transaction = DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_CLIENT_NON_INVITE);

    POINTERS_EQUAL(transaction, GetTransaction(MessageGetViaBranch(invite), MessageGetCSeqMethod(invite)));
}

TEST(DialogTestGroup, UACDialogIdTest)
{
    char okString[MAX_MESSAGE_LENGTH] = {0};
    MESSAGE *originInvite = BuildInviteMessage(dialog, (char *)"88002");
    MESSAGE *localOk = Build200OkMessage(NULL, originInvite);
    
    Message2String(okString, localOk);
    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_INVITE));
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(okString);    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_ACK));

    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_CLIENT_INVITE);
    ReceiveMessage();
    
    STRCMP_EQUAL(MessageGetFromTag(originInvite), GetLocalTag(DialogGetId(dialog)));     
    STRCMP_EQUAL(MessageGetCallId(originInvite), GetCallId(DialogGetId(dialog)));
    STRCMP_EQUAL(MessageGetToTag(localOk), GetRemoteTag(DialogGetId(dialog)));    

    DestroyMessage(&originInvite);
    DestroyMessage(&localOk);
}

TEST(DialogTestGroup, UACDialogIdDelegateTest)
{
    char okString[MAX_MESSAGE_LENGTH] = {0};
    MESSAGE *originInvite = BuildInviteMessage(dialog, (char *)"88002");
    MESSAGE *localOk = Build200OkMessage(NULL, originInvite);
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

TEST(DialogTestGroup, UACDialogLocalSeqNumberTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_INVITE));
    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_CLIENT_INVITE);

    CHECK_EQUAL(MessageGetCSeqNumber(invite), DialogGetLocalSeqNumber(dialog));
}

TEST(DialogTestGroup, UACDialogRemoteSeqNumberTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_INVITE));
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(OK_MESSAGE_RECEIVED);    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_ACK));
   
    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_CLIENT_INVITE);
    ReceiveMessage();

    CHECK_EQUAL(EMPTY_DIALOG_SEQNUMBER, DialogGetRemoteSeqNumber(dialog));
}

TEST(DialogTestGroup, UACDialogRemoteTargetTest)
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

TEST(DialogTestGroup, UACDialogRemoteUriTest)
{
    
    URI *remoteUri = DialogGetRemoteUri(dialog);
    CONTACT_HEADER *to = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_TO, invite);

    UriMatched(remoteUri, ContactHeaderGetUri(to));
    DestroyMessage(&invite);
}

TEST(DialogTestGroup, UACDialogConfirmedTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_INVITE));
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(OK_MESSAGE_RECEIVED);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_ACK));
   
    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_CLIENT_INVITE);
    ReceiveMessage();

    CHECK_EQUAL(DIALOG_STATE_CONFIRMED, DialogGetState(dialog));

}

TEST(DialogTestGroup, UASDialog200OkMessageDestTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200).
        withStringParameter("destaddr", AccountGetProxyAddr(GetAccount(0))).
        withIntParameter("destport", AccountGetProxyPort(GetAccount(0)));

    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_SERVER_INVITE);
    
    UT_PTR_SET(SipTransporter, &MockTransporterAndHandle);
    DialogOk(dialog);
}

TEST(DialogTestGroup, UACDialogTerminateTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_BYE));
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);

    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_SERVER_INVITE);
    DialogOk(dialog);
    DialogTerminate(dialog);

    CHECK_EQUAL(DIALOG_STATE_TERMINATED, DialogGetState(dialog));
}

TEST(DialogTestGroup, UASDialogIdTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);
    CONTACT_HEADER *from = ContactHeaderDup((CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_FROM, invite));
    struct CallIdHeader *id = CallIdHeaderDup((struct CallIdHeader *)MessageGetHeader(HEADER_NAME_CALLID, invite));

    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_SERVER_INVITE);
    MESSAGE *ok = Build200OkMessage(NULL, invite);
    DialogOk(dialog);
    
    STRCMP_EQUAL(MessageGetToTag(ok), GetLocalTag(DialogGetId(dialog)));
    STRCMP_EQUAL(ContactHeaderGetParameter(from, HEADER_PARAMETER_NAME_TAG), GetRemoteTag(DialogGetId(dialog)));
    STRCMP_EQUAL(CallIdHeaderGetId(id), GetCallId(DialogGetId(dialog)));

    DestroyContactHeader((struct Header *)from);
    DestroyCallIdHeader((struct Header *)id);
    DestroyMessage(&ok);
}

TEST(DialogTestGroup, UASDialogConfirmedTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);

    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_SERVER_INVITE);
    DialogOk(dialog);

    CHECK_EQUAL(DIALOG_STATE_CONFIRMED, DialogGetState(dialog));
}

TEST(DialogTestGroup, UASDialogRemoteSeqNumberTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);

    unsigned int seq = MessageGetCSeqNumber(invite);
    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_SERVER_INVITE);
    DialogOk(dialog);

    CHECK_EQUAL(seq, DialogGetRemoteSeqNumber(dialog));
}

TEST(DialogTestGroup, UASDialogLocalSeqNumberTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);

    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_SERVER_INVITE);
    DialogOk(dialog);

    CHECK_EQUAL(CSeqGenerateSeq(), DialogGetLocalSeqNumber(dialog));
}

TEST(DialogTestGroup, UASDialogRemoteTargetTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);

    CONTACT_HEADER *c = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_CONTACT, invite);
    URI *uri = UriDup(ContactHeaderGetUri(c));
    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_SERVER_INVITE);
    DialogOk(dialog);

    CHECK_TRUE(UriMatched(uri, DialogGetRemoteTarget(dialog)));
    DestroyUri(&uri);
}

TEST(DialogTestGroup, UASDialogTerminateTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);

    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_SERVER_INVITE); 
    DialogOk(dialog);

    MESSAGE *bye = BuildByeMessage(dialog);
    DialogReceiveBye(dialog, bye);
    CHECK_EQUAL(DIALOG_STATE_TERMINATED, DialogGetState(dialog));
}

TEST(DialogTestGroup, ReceiveInviteTest)
{
    ClearDialogManager();
    
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("StatusCode", 100);
    ReceiveMessage();
    CHECK_EQUAL(1, CountDialogs());

    DestroyMessage(&invite);
    ClearUserAgentManager();
}
