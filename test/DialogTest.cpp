#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TestingMessages.h"
#include "TransportMock.h"

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
#include "MessageTransport.h"
#include "RequestLine.h"
}

TEST_GROUP(DialogTestGroup)
{
    struct UserAgent *ua;
    struct Dialog *dialog;
    void setup()
    {
        UT_PTR_SET(Transporter, &MockTransporter);
        UT_PTR_SET(ReceiveMessageCallback, MessageReceived);

        ua = BuildUserAgent();
        dialog = CreateDialog(NULL_DIALOG_ID, ua);

    }

    void teardown()
    {
        EmptyTransactionManager();
        DestoryUserAgent(&ua);
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
    DestoryMessage(&m);

    return 0;
}

static struct MessageTransporter MockTransporterForAck = {
    SendOutAckMessageMock,
    ReceiveInMessageMock,
};

TEST(DialogTestGroup, AckRequestAfterInviteSuccessedTest)
{
    char revMessage[MAX_MESSAGE_LENGTH] = {0};
    struct Message *invite = BuildInviteMessage(dialog);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_200OK_MESSAGE);    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("RemoteTag", "as6151ad25");
   
    AddClientTransaction(invite, (struct TransactionUserNotifiers *)dialog);
    UT_PTR_SET(Transporter, &MockTransporterForAck);

    ReceiveInMessage(revMessage);
}

TEST(DialogTestGroup, AddTransactionTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);

    struct Message *invite = BuildInviteMessage(dialog);
    struct Transaction *transaction = DialogAddClientTransaction(dialog, invite);

    POINTERS_EQUAL(transaction, GetTransaction(MessageGetViaBranch(invite), MessageGetCSeqMethod(invite)));
}

TEST(DialogTestGroup, UACDialogIdTest)
{
    char revMessage[MAX_MESSAGE_LENGTH] = {0};
    struct Message *invite = BuildInviteMessage(dialog);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_200OK_MESSAGE);    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
   
    struct Transaction *t = DialogAddClientTransaction(dialog, invite);
    ReceiveInMessage(revMessage);

    STRCMP_EQUAL(MessageGetFromTag(invite), DialogIdGetLocalTag(DialogGetId(dialog)));    
    STRCMP_EQUAL(MessageGetCallId(invite), DialogIdGetCallId(DialogGetId(dialog)));
    STRCMP_EQUAL(MessageGetToTag(TransactionGetLatestResponse(t)), DialogIdGetRemoteTag(DialogGetId(dialog)));    

}

TEST(DialogTestGroup, UASDialogIdTest)
{
    struct Message *invite = BuildInviteMessage(dialog);
    struct Message *ok = Build200OKMessage(invite);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);

    DialogAddServerTransaction(dialog, invite);
    DialogSend200OKResponse(dialog);

    STRCMP_EQUAL(MessageGetToTag(ok), DialogIdGetLocalTag(DialogGetId(dialog)));
    STRCMP_EQUAL(MessageGetFromTag(invite), DialogIdGetRemoteTag(DialogGetId(dialog)));
    STRCMP_EQUAL(MessageGetCallId(invite), DialogIdGetCallId(DialogGetId(dialog)));

    DestoryMessage(&ok);
}

TEST(DialogTestGroup, UACDialogLocalSeqNumberTest)
{
    struct Message *invite = BuildInviteMessage(dialog);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
    DialogAddClientTransaction(dialog, invite);

    CHECK_EQUAL(MessageGetCSeqNumber(invite), DialogGetLocalSeqNumber(dialog));
}

TEST(DialogTestGroup, UACDialogRemoteSeqNumberTest)
{
    char revMessage[MAX_MESSAGE_LENGTH] = {0};
    struct Message *invite = BuildInviteMessage(dialog);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_200OK_MESSAGE);    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
   
    DialogAddClientTransaction(dialog, invite);
    ReceiveInMessage(revMessage);

    CHECK_EQUAL(0, DialogGetRemoteSeqNumber(dialog));
}

TEST(DialogTestGroup, UACDialogConfirmedTest)
{
    char revMessage[MAX_MESSAGE_LENGTH] = {0};
    struct Message *invite = BuildInviteMessage(dialog);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_200OK_MESSAGE);    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
   
    DialogAddClientTransaction(dialog, invite);
    ReceiveInMessage(revMessage);

    CHECK_EQUAL(DIALOG_STATE_CONFIRMED, DialogGetState(dialog));

}
