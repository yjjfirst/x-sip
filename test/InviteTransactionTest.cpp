#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TransportMock.h"

extern "C" {
#include <stdio.h>
#include <string.h>
#include "Messages.h"
#include "MessageBuilder.h"
#include "UserAgent.h"
#include "TransactionManager.h"
#include "Transaction.h"
#include "MessageTransport.h"
#include "TestingMessages.h"
#include "CallIdHeader.h"
#include "Dialog.h"
#include "Provision.h"
}

static struct Timer *AddTimerMock(void *p, int ms, TimerCallback onTime)
{
    mock().actualCall("AddTimer");
    return NULL;
}


TEST_GROUP(OutgoingInviteTransactionTestGroup)
{
    struct UserAgent *ua;
    struct Message *message;
    struct Transaction *t;
    struct Dialog *dialog;
    void setup(){
        mock().expectOneCall("AddTimer");
        mock().expectOneCall("AddTimer");
        mock().expectOneCall("SendOutMessageMock");

        UT_PTR_SET(ReceiveMessageCallback, MessageReceived);
        UT_PTR_SET(AddTimer, AddTimerMock);
        UT_PTR_SET(Transporter, &MockTransporter);

        ua = BuildUserAgent();
        dialog = CreateDialog(NULL, ua);
        message = BuildInviteMessage(dialog); 
        t = AddClientTransaction(message,(struct TransactionUser *) dialog);

   }

    void teardown() {
        DestoryUserAgent(&ua);
        EmptyTransactionManager();
        mock().checkExpectations();
        mock().clear();
    }

    struct UserAgent *BuildUserAgent()
    {
        struct UserAgent *ua = CreateUserAgent();

        UserAgentSetUserName(ua, GetUserName());
        UserAgentSetRegistrar(ua, GetRegistrar());
        UserAgentSetProxy(ua, GetProxy());

        return ua;

    }
};

TEST(OutgoingInviteTransactionTestGroup, CreateInviteTransaction)
{ 
    CHECK_EQUAL(TRANSACTION_STATE_CALLING, TransactionGetState(t));
}

TEST(OutgoingInviteTransactionTestGroup, Receive2xxTest)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(INVITE_200OK_MESSAGE);
    mock().expectOneCall("AddTimer");

    mock().expectOneCall("AddTimer");
    mock().expectOneCall("AddTimer");
    mock().expectOneCall("SendOutMessageMock");
    
    ReceiveInMessage(stringReceived);
    
    POINTERS_EQUAL(NULL, GetTransaction((char *)"z9hG4bK1491280923", (char *)SIP_METHOD_NAME_INVITE));

    mock().checkExpectations();
}

TEST(OutgoingInviteTransactionTestGroup, Receive100Test)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(INVITE_100TRYING_MESSAGE);
    ReceiveInMessage(stringReceived);
    
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));
    POINTERS_EQUAL(t, GetTransaction((char *)"z9hG4bK1491280923", (char *)SIP_METHOD_NAME_INVITE));

    mock().checkExpectations();
}

TEST(OutgoingInviteTransactionTestGroup, Receive180Test)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(INVITE_100TRYING_MESSAGE);
    ReceiveInMessage(stringReceived);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(INVITE_180RINGING_MESSAGE);
    ReceiveInMessage(stringReceived);    
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));

    mock().checkExpectations();
}

TEST(OutgoingInviteTransactionTestGroup, Receive100and180and200Test)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(INVITE_100TRYING_MESSAGE);
    mock().expectOneCall("AddTimer");
    ReceiveInMessage(stringReceived);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(INVITE_180RINGING_MESSAGE);
    ReceiveInMessage(stringReceived);    
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(INVITE_200OK_MESSAGE);
    ReceiveInMessage(stringReceived);    
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t));

    mock().checkExpectations();
}

