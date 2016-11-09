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
    }

    void teardown()
    {
        ClearDialogManager();
        ClearUaManager();
        ClearAccountManager();
        ClearTransactionManager();

        mock().checkExpectations();
        mock().clear();
    }
};


TEST(UASDialogTestGroup, UASDialog200OkMessageDestTest)
{
    invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200).
        withStringParameter("destaddr", AccountGetProxyAddr(GetAccount(0))).
        withIntParameter("destport", AccountGetProxyPort(GetAccount(0)));

    OnTransactionEvent(NULL, TRANSACTION_EVENT_NEW, invite);

    UT_PTR_SET(SipTransporter, &MockTransporterAndHandle);
    DialogOk(GetDialog(0));

    ClearUaManager();
}

TEST(UASDialogTestGroup, UASDialogIdTest)
{
    invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);
    CONTACT_HEADER *from = ContactHeaderDup((CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_FROM, invite));
    struct CallIdHeader *id = CallIdHeaderDup((struct CallIdHeader *)MessageGetHeader(HEADER_NAME_CALLID, invite));

    OnTransactionEvent(NULL, TRANSACTION_EVENT_NEW, invite);
    MESSAGE *ok = BuildResponse(NULL, invite, STATUS_CODE_OK);
    DialogOk(GetDialog(0));
    
    STRCMP_EQUAL(MessageGetToTag(ok), GetLocalTag(GetDialogId(GetDialog(0))));
    STRCMP_EQUAL(ContactHeaderGetParameter(from, HEADER_PARAMETER_NAME_TAG), GetRemoteTag(GetDialogId(GetDialog(0))));
    STRCMP_EQUAL(CallIdHeaderGetId(id), GetCallId(GetDialogId(GetDialog(0))));

    DestroyContactHeader((struct Header *)from);
    DestroyCallIdHeader((struct Header *)id);
    DestroyMessage(&ok);
}

TEST(UASDialogTestGroup, UASDialogConfirmedStateTest)
{
    invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);

    OnTransactionEvent(NULL, TRANSACTION_EVENT_NEW, invite);
    DialogOk(GetDialog(0));

    CHECK_EQUAL(DIALOG_STATE_CONFIRMED, GetDialogState(GetDialog(0)));
}

TEST(UASDialogTestGroup, UASDialogRemoteSeqNumberTest)
{
    invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);

    unsigned int seq = MessageGetCSeqNumber(invite);

    OnTransactionEvent(NULL, TRANSACTION_EVENT_NEW, invite);
    DialogOk(GetDialog(0));

    CHECK_EQUAL(seq, GetRemoteSeqNumber(GetDialog(0)));
}

TEST(UASDialogTestGroup, UASDialogLocalSeqNumberTest)
{
    invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);

    OnTransactionEvent(NULL, TRANSACTION_EVENT_NEW, invite);
    unsigned int cseq =  MessageGetCSeqNumber(invite);
    DialogOk(GetDialog(0));

    CHECK_EQUAL(cseq, GetLocalSeqNumber(GetDialog(0)));
}

TEST(UASDialogTestGroup, UASDialogRemoteTargetTest)
{
    invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);

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
    invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);

    OnTransactionEvent(NULL, TRANSACTION_EVENT_NEW, invite);
    DialogOk(GetDialog(0));

    MESSAGE *bye = BuildByeMessage(GetDialog(0));
    DialogReceiveBye(GetDialog(0), bye);
    CHECK_EQUAL(0, CountDialogs());
}

TEST(UASDialogTestGroup, MatchedRequestToDialog)
{
    invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 200);

    OnTransactionEvent(NULL, TRANSACTION_EVENT_NEW, invite);
    DialogOk(GetDialog(0));

}

TEST(UASDialogTestGroup, ReceiveInviteCountDialogTest)
{
    UT_PTR_SET(AddTransaction, AddTransactionMock);

    invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);

    mock().expectOneCall("AddTransaction").withParameter("message", invite).
        withParameter("type", TRANSACTION_TYPE_SERVER_INVITE);

    OnTransactionEvent(NULL, TRANSACTION_EVENT_NEW, invite);
    CHECK_EQUAL(1, CountDialogs());

    DestroyMessage(&invite);
}

TEST(UASDialogTestGroup, ReceiveInviteTest)
{
    invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);

    UT_PTR_SET(AddTransaction, AddTransactionMock);

    mock().expectOneCall("AddTransaction").withParameter("message", invite).
        withParameter("type", TRANSACTION_TYPE_SERVER_INVITE);

    OnTransactionEvent(NULL, TRANSACTION_EVENT_NEW, invite);
    CHECK_EQUAL(1, CountDialogs());
    DestroyMessage(&invite);
}

TEST(UASDialogTestGroup, ReceiveByeTest)
{
    struct Message *bye = CreateMessage();
    ParseMessage(INCOMMING_BYE_MESSAGE,bye);

    struct UserAgent *ua = AddUa(0);
    struct DialogId *id = CreateFixedDialogId((char *)"1312549293",
                                              (char *)"as317b5f26",
                                              (char *)"2074940689");
    AddConfirmedDialog(id, ua);    
    mock().expectOneCall("SendOutMessageMock").withParameter("StatusCode", 200);
    OnTransactionEvent(NULL, TRANSACTION_EVENT_NEW, bye);

    CHECK_EQUAL(0, CountDialogs());
}

void RemoveUaMock(struct UserAgent *ua)
{
    mock().actualCall("RemoveUa").withParameter("ua", ua);
}

TEST(UASDialogTestGroup, ReceiveByeRemoveDialogTest)
{
    UT_PTR_SET(RemoveUa, RemoveUaMock);
    
    struct Message *bye = CreateMessage();
    ParseMessage(INCOMMING_BYE_MESSAGE,bye);

    struct UserAgent *ua = AddUa(0);
    struct DialogId *id = CreateFixedDialogId((char *)"1312549293",
                                              (char *)"as317b5f26",
                                              (char *)"2074940689");
    AddConfirmedDialog(id, ua);
    
    mock().expectOneCall("RemoveUa").withParameter("ua", ua);
    mock().expectOneCall("SendOutMessageMock").withParameter("StatusCode", 200);
    OnTransactionEvent(NULL, TRANSACTION_EVENT_NEW, bye);    
}

TEST(UASDialogTestGroup, ReceiveByeNotifyCmTest)
{
    UT_PTR_SET(NotifyCm, NotifyCallManagerMock);
    
    struct Message *bye = CreateMessage();
    ParseMessage(INCOMMING_BYE_MESSAGE,bye);

    struct UserAgent *ua = AddUa(0);
    struct DialogId *id = CreateFixedDialogId((char *)"1312549293",
                                              (char *)"as317b5f26",
                                              (char *)"2074940689");
    AddConfirmedDialog(id, ua);

    mock().expectOneCall("NotifyCm").
        withParameter("event", CALL_FINISHED).
        withParameter("ua", ua);
    
    mock().expectOneCall("SendOutMessageMock").withParameter("StatusCode", 200);
    OnTransactionEvent(NULL, TRANSACTION_EVENT_NEW, bye);    
}

TEST(UASDialogTestGroup, RingingTest)
{
    struct UserAgent *ua = AddUa(0);
    struct Dialog *dialog = AddDialog(NULL, ua);

    invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_SERVER_INVITE);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 180);
    DialogRinging(dialog);    
}

IGNORE_TEST(UASDialogTestGroup, RemoteCancelTest)
{
    struct UserAgent *ua = AddUa(0);
    struct Dialog *dialog = AddDialog(NULL, ua);
    
    struct Message *cancel = CreateMessage();
    ParseMessage(INCOMMING_CANCEL_MESSAGE, cancel);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 481);
    DialogNewTransaction(dialog, cancel, TRANSACTION_TYPE_SERVER_NON_INVITE);
    
}
