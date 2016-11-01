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

static struct Transaction *AddTransactionMock(MESSAGE *message, struct TransactionUser *user, int type)
{
    mock().actualCall("AddTransaction").withParameter("message", message).
        withParameter("type", type); 

    return NULL;
}

TEST_GROUP(UASDialogTestGroup)
{
    MESSAGE *invite;
    void setup()
    {
        UT_PTR_SET(SipTransporter, &MockTransporter);

        AccountInit();
        invite = CreateMessage();
        ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    }

    void teardown()
    {
        ClearDialogManager();
        ClearUserAgentManager();
        ClearAccountManager();
        ClearTransactionManager();

        mock().checkExpectations();
        mock().clear();
    }
};


TEST(UASDialogTestGroup, UASDialog200OkMessageDestTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200).
        withStringParameter("destaddr", AccountGetProxyAddr(GetAccount(0))).
        withIntParameter("destport", AccountGetProxyPort(GetAccount(0)));

    OnTransactionEvent(NULL, TRANSACTION_EVENT_NEW, invite);

    UT_PTR_SET(SipTransporter, &MockTransporterAndHandle);
    DialogOk(GetDialog(0));

    ClearUserAgentManager();
}

TEST(UASDialogTestGroup, UASDialogIdTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);
    CONTACT_HEADER *from = ContactHeaderDup((CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_FROM, invite));
    struct CallIdHeader *id = CallIdHeaderDup((struct CallIdHeader *)MessageGetHeader(HEADER_NAME_CALLID, invite));

    OnTransactionEvent(NULL, TRANSACTION_EVENT_NEW, invite);
    MESSAGE *ok = Build200OkMessage(NULL, invite);
    DialogOk(GetDialog(0));
    
    STRCMP_EQUAL(MessageGetToTag(ok), GetLocalTag(DialogGetId(GetDialog(0))));
    STRCMP_EQUAL(ContactHeaderGetParameter(from, HEADER_PARAMETER_NAME_TAG), GetRemoteTag(DialogGetId(GetDialog(0))));
    STRCMP_EQUAL(CallIdHeaderGetId(id), GetCallId(DialogGetId(GetDialog(0))));

    DestroyContactHeader((struct Header *)from);
    DestroyCallIdHeader((struct Header *)id);
    DestroyMessage(&ok);
}

TEST(UASDialogTestGroup, UASDialogConfirmedTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);

    OnTransactionEvent(NULL, TRANSACTION_EVENT_NEW, invite);
    DialogOk(GetDialog(0));

    CHECK_EQUAL(DIALOG_STATE_CONFIRMED, DialogGetState(GetDialog(0)));
}

TEST(UASDialogTestGroup, UASDialogRemoteSeqNumberTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);

    unsigned int seq = MessageGetCSeqNumber(invite);

    OnTransactionEvent(NULL, TRANSACTION_EVENT_NEW, invite);
    DialogOk(GetDialog(0));

    CHECK_EQUAL(seq, DialogGetRemoteSeqNumber(GetDialog(0)));
}

TEST(UASDialogTestGroup, UASDialogLocalSeqNumberTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);

    OnTransactionEvent(NULL, TRANSACTION_EVENT_NEW, invite);
    unsigned int cseq =  MessageGetCSeqNumber(invite);
    DialogOk(GetDialog(0));

    CHECK_EQUAL(cseq, DialogGetLocalSeqNumber(GetDialog(0)));
}

TEST(UASDialogTestGroup, UASDialogRemoteTargetTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);

    CONTACT_HEADER *c = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_CONTACT, invite);
    URI *uri = UriDup(ContactHeaderGetUri(c));

    OnTransactionEvent(NULL, TRANSACTION_EVENT_NEW, invite);
    DialogOk(GetDialog(0));

    CHECK_TRUE(UriMatched(uri, DialogGetRemoteTarget(GetDialog(0))));
    DestroyUri(&uri);
}

TEST(UASDialogTestGroup, UASDialogTerminateTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);

    OnTransactionEvent(NULL, TRANSACTION_EVENT_NEW, invite);
    DialogOk(GetDialog(0));

    MESSAGE *bye = BuildByeMessage(GetDialog(0));
    DialogReceiveBye(GetDialog(0), bye);
    CHECK_EQUAL(DIALOG_STATE_TERMINATED, DialogGetState(GetDialog(0)));
}

TEST(UASDialogTestGroup, ReceiveInviteCountDialogTest)
{
    UT_PTR_SET(AddTransaction, AddTransactionMock);

    mock().expectOneCall("AddTransaction").withParameter("message", invite).
        withParameter("type", TRANSACTION_TYPE_SERVER_INVITE);

    OnTransactionEvent(NULL, TRANSACTION_EVENT_NEW, invite);
    CHECK_EQUAL(1, CountDialogs());

    DestroyMessage(&invite);
}

TEST(UASDialogTestGroup,ReceiveInviteAddTransactionTest)
{
    UT_PTR_SET(AddTransaction, AddTransactionMock);

    mock().expectOneCall("AddTransaction").withParameter("message", invite).
        withParameter("type", TRANSACTION_TYPE_SERVER_INVITE);

    OnTransactionEvent(NULL, TRANSACTION_EVENT_NEW, invite);
    DestroyMessage(&invite);
}

