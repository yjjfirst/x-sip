#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include "Messages.h"
#include "MessageBuilder.h"
#include "UserAgent.h"
#include "TransactionManager.h"
#include "Transaction.h"
#include "MessageTransport.h"
}

static int ReceiveMessageMock(char *message)
{
    return 0;
}

static int SendMessageMock(char *message)
{
    return 0;
}

static void AddTimer(void *p, int ms, TimerCallback onTime)
{

}

TEST_GROUP(InviteTransactionTestGroup)
{
    void setup(){
        AddMessageTransporter((char *)"TRANS", SendMessageMock, ReceiveMessageMock);
        TransactionSetTimer(AddTimer);
    }

    void teardown() {
        RemoveMessageTransporter((char *)"TRANS");
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
    DestoryTransactionManager();
}


TEST(InviteTransactionTestGroup, Receive2xxTest)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};
    struct UserAgent *ua = BuildUserAgent();
    struct Message *message = BuildInviteMessage(ua, (char *)"88002");
    CreateTransactionExt(message,(struct TransactionOwnerInterface *) ua);
    
    ReceiveMessage(stringReceived);
    DestoryUserAgent(&ua);
    DestoryTransactionManager();
}
