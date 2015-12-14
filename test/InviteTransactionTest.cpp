#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

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
}

static int ReceiveMessageMock(char *message)
{
    strcpy(message, mock().actualCall("ReceiveMessageMock").returnStringValue());
    return 0;
}

static int SendMessageMock(char *message)
{
    return 0;
}

static struct Timer *AddTimer(void *p, int ms, TimerCallback onTime)
{
    mock().actualCall("AddTimer");
    return NULL;
}

TEST_GROUP(InviteTransactionTestGroup)
{
    void setup(){
        mock().expectOneCall("AddTimer");
        mock().expectOneCall("AddTimer");

        AddMessageTransporter((char *)"TRANS", SendMessageMock, ReceiveMessageMock);
        TransactionSetTimerManager(AddTimer);
        InitReceiveMessageCallback(MessageReceived);
    }

    void teardown() {
        DestoryTransactionManager();
        RemoveMessageTransporter((char *)"TRANS");
        TransactionRemoveTimer();
        mock().checkExpectations();
        mock().clear();
    }

    struct UserAgent *BuildUserAgent()
    {
        struct UserAgent *ua = CreateUserAgent();

        UserAgentSetUserName(ua, (char *)"88001");
        UserAgentSetRegistrar(ua, (char *)"192.168.10.63");
        UserAgentSetProxy(ua, (char *)"192.168.10.63");

        return ua;

    }
};

TEST(InviteTransactionTestGroup, CreateInviteTransaction)
{
    struct UserAgent *ua = BuildUserAgent();
    struct Message *message = BuildInviteMessage(ua, (char *)"88002");
    struct Transaction *t = CreateTransactionExt(message,(struct TransactionOwnerInterface *) ua);
 
    CHECK_EQUAL(TRANSACTION_STATE_CALLING, TransactionGetState(t));

    DestoryUserAgent(&ua);
}

TEST(InviteTransactionTestGroup, Receive2xxTest)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};
    struct UserAgent *ua = BuildUserAgent();
    struct Message *message = BuildInviteMessage(ua, (char *)"88002");
    CreateTransactionExt(message,(struct TransactionOwnerInterface *) ua);    

    mock().expectOneCall("ReceiveMessageMock").andReturnValue(INVITE_200OK_MESSAGE);
    mock().expectOneCall("AddTimer");
    ReceiveMessage(stringReceived);
    
    CHECK_EQUAL(0, CountTransaction());
    POINTERS_EQUAL(NULL, GetTransactionBy((char *)"z9hG4bK1491280923", (char *)SIP_METHOD_NAME_INVITE));

    DestoryUserAgent(&ua);
    mock().checkExpectations();
}

TEST(InviteTransactionTestGroup, Receive100Test)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};
    struct UserAgent *ua = BuildUserAgent();
    struct Message *message = BuildInviteMessage(ua, (char *)"88002");
    struct Transaction *t = CreateTransactionExt(message,(struct TransactionOwnerInterface *) ua);    

    mock().expectOneCall("ReceiveMessageMock").andReturnValue(INVITE_100TRYING_MESSAGE);
    ReceiveMessage(stringReceived);
    
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));
    POINTERS_EQUAL(t, GetTransactionBy((char *)"z9hG4bK1491280923", (char *)SIP_METHOD_NAME_INVITE));

    DestoryUserAgent(&ua);
    mock().checkExpectations();
}

TEST(InviteTransactionTestGroup, Receive180Test)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};
    struct UserAgent *ua = BuildUserAgent();
    struct Message *message = BuildInviteMessage(ua, (char *)"88002");
    struct Transaction *t = CreateTransactionExt(message,(struct TransactionOwnerInterface *) ua);    

    mock().expectOneCall("ReceiveMessageMock").andReturnValue(INVITE_100TRYING_MESSAGE);
    ReceiveMessage(stringReceived);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));

    mock().expectOneCall("ReceiveMessageMock").andReturnValue(INVITE_180RINGING_MESSAGE);
    ReceiveMessage(stringReceived);    
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));

    DestoryUserAgent(&ua);
    mock().checkExpectations();

}

TEST(InviteTransactionTestGroup, Receive100and180and200Test)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};
    struct UserAgent *ua = BuildUserAgent();
    struct Message *message = BuildInviteMessage(ua, (char *)"88002");
    struct Transaction *t = CreateTransactionExt(message,(struct TransactionOwnerInterface *) ua);    

    mock().expectOneCall("ReceiveMessageMock").andReturnValue(INVITE_100TRYING_MESSAGE);
    mock().expectOneCall("AddTimer");
    ReceiveMessage(stringReceived);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));

    mock().expectOneCall("ReceiveMessageMock").andReturnValue(INVITE_180RINGING_MESSAGE);
    ReceiveMessage(stringReceived);    
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));

    mock().expectOneCall("ReceiveMessageMock").andReturnValue(INVITE_200OK_MESSAGE);
    ReceiveMessage(stringReceived);    
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t));

    DestoryUserAgent(&ua);
    mock().checkExpectations();
}
