#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include <string.h>
#include "Messages.h"
#include "MessageBuilder.h"
#include "UserAgent.h"
#include "TransactionManager.h"
#include "Transaction.h"
#include "MessageTransport.h"
#include "Timer.h"
}

#define Message200OK "SIP/2.0 200 OK\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5061;branch=z9hG4bK1491280923;received=192.168.10.1;rport=5061\r\n\
From: <sip:88001@192.168.10.62>;tag=1296642367\r\n\
To: <sip:88002@192.168.10.62>;tag=as6151ad25\r\n\
Call-ID: 97295390\r\n\
CSeq: 20 INVITE\r\n\
Contact: <sip:88002@192.168.10.62:5060>\r\n\
Content-Type: application/sdp\r\n\
Content-Length: 289\r\n"

#define Message100Trying "SIP/2.0 100 Trying\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5061;branch=z9hG4bK1491280923;received=192.168.10.1;rport=5061\r\n\
From: <sip:88001@192.168.10.62>;tag=1226271270\r\n\
To: <sip:88002@192.168.10.62>\r\n\
Call-ID: 778885328\r\n\
CSeq: 20 INVITE\r\n\
Contact: <sip:88002@192.168.10.62:5060>\r\n\
Content-Length: 0\r\n"

#define Message180Ringing "SIP/2.0 180 Ringing\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5061;branch=z9hG4bK1441229791;received=192.168.10.1;rport=5061\r\n\
From: <sip:88001@192.168.10.62>;tag=1226271270\r\n\
To: <sip:88002@192.168.10.62>;tag=as5cde26a4\r\n\
Call-ID: 778885328\r\n\
CSeq: 20 INVITE\r\n\
Contact: <sip:88002@192.168.10.62:5060>\r\n\
Content-Length: 0\r\n"

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

    mock().expectOneCall("ReceiveMessageMock").andReturnValue(Message200OK);
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

    mock().expectOneCall("ReceiveMessageMock").andReturnValue(Message100Trying);
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

    mock().expectOneCall("ReceiveMessageMock").andReturnValue(Message100Trying);
    ReceiveMessage(stringReceived);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));

    mock().expectOneCall("ReceiveMessageMock").andReturnValue(Message180Ringing);
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

    mock().expectOneCall("ReceiveMessageMock").andReturnValue(Message100Trying);
    mock().expectOneCall("AddTimer");
    ReceiveMessage(stringReceived);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));

    mock().expectOneCall("ReceiveMessageMock").andReturnValue(Message180Ringing);
    ReceiveMessage(stringReceived);    
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, TransactionGetState(t));

    mock().expectOneCall("ReceiveMessageMock").andReturnValue(Message200OK);
    ReceiveMessage(stringReceived);    
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t));

    DestoryUserAgent(&ua);
    mock().checkExpectations();
}

TEST(InviteTransactionTestGroup, RetransmitTest)
{
    struct UserAgent *ua = BuildUserAgent();
    struct Message *message = BuildInviteMessage(ua, (char *)"88002");
    CreateTransactionExt(message,(struct TransactionOwnerInterface *) ua);

    DestoryUserAgent(&ua);
    mock().checkExpectations();

}
