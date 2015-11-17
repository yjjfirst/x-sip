#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include <stdio.h>
#include <string.h>
#include "Messages.h"
#include "TransactionManager.h"
#include "MessageBuilder.h"
#include "MessageTransport.h"
#include "Transaction.h"
}

static char OKMessage[] = "\
SIP/2.0 200 OK\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5060;branch=z9hG4bK1491280923;received=192.168.10.1;rport=5060\r\n\
From: <sip:88001@192.168.10.62>;tag=1225432999\r\n\
To: <sip:88001@192.168.10.62>;tag=as1d07559a\r\n\
Call-ID: 1222971951\r\n\
CSeq: 1 REGISTER\r\n\
Expires:3600\r\n\
Contact: <sip:88001@192.168.10.1;line=f2fd53ebfa7728f>;expires=3600\r\n\
Content-Length: 0\r\n";

static char TryingMessage[] = "\
SIP/2.0 100 Trying\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5060;branch=z9hG4bK1491280923;received=192.168.10.1;rport=5060\r\n\
From: <sip:88001@192.168.10.62>;tag=1225432999\r\n\
To: <sip:88001@192.168.10.62>;tag=as1d07559a\r\n\
Call-ID: 1222971951\r\n\
CSeq: 1 REGISTER\r\n\
Expires:3600\r\n\
Contact: <sip:88001@192.168.10.1;line=f2fd53ebfa7728f>;expires=3600\r\n\
Content-Length: 0\r\n";

enum Response {
    OK200,
    RINGING180,
};

static enum Response Response;

static int ReceiveMessageMock(char *message)
{
    if (Response == RINGING180)
        strcpy(message, TryingMessage);
    else
        strcpy(message, OKMessage);
    return 0;
}

static int SendMessageMock(char *message)
{
    return 0;
}

TEST_GROUP(TransactionManager)
{
    void setup() {
        AddMessageTransporter((char *)"TRANS", SendMessageMock, ReceiveMessageMock);
        InitReceiveMessageCallback(MessageReceived);
    }

    void teardown() {
        RemoveMessageTransporter((char *)"TRANS");
    }
};

TEST(TransactionManager, Init)
{
    struct TransactionManager *manager = GetTransactionManager();
    CHECK_FALSE(manager == NULL);

    DestoryTransactionManager(&manager);
    CHECK_TRUE(manager == NULL);    
}

TEST(TransactionManager, Signleton)
{
    struct TransactionManager *m1 = GetTransactionManager();
    struct TransactionManager *m2 = GetTransactionManager();

    CHECK_EQUAL(m1, m2);

    DestoryTransactionManager(&m1);
}

TEST(TransactionManager, NewTransaction)
{
    struct Message *message = BuildRegisterMessage();
    struct TransactionManager *manager = GetTransactionManager();
    struct Transaction *transaction;

    transaction = manager->CreateTransaction(message);
    CHECK_EQUAL(1, manager->CountTransaction());

    message = BuildRegisterMessage();
    transaction = manager->CreateTransaction(message);
    CHECK_EQUAL(2, manager->CountTransaction());

    message = BuildRegisterMessage();
    transaction = manager->CreateTransaction(message);
    CHECK_EQUAL(3, manager->CountTransaction());


    CHECK_FALSE(0 == transaction)
    DestoryTransactionManager(&manager);
}

TEST(TransactionManager, MatchResponse)
{
    struct Message *message = BuildRegisterMessage();
    struct TransactionManager *manager = GetTransactionManager();
    char string[MAX_MESSAGE_LENGTH] = {0};
    
    MessageAddViaParameter(message, (char *)"rport", (char *)"");
    MessageAddViaParameter(message, (char *)"branch", (char *)"z9hG4bK1491280923");

    manager->CreateTransaction(message);
    CHECK_TRUE(ReceiveMessage(string));

    DestoryTransactionManager(&manager);
}

TEST(TransactionManager, BranchNonMatchTest)
{
    char string[MAX_MESSAGE_LENGTH] = {0};
    struct Message *message = BuildRegisterMessage();
    struct TransactionManager *manager = GetTransactionManager();
    struct Transaction *t = manager->CreateTransaction(message);
    enum TransactionState s;

    MessageAddViaParameter(message, (char *)"branch", (char *)"z9hG4bK1491280924");

    ReceiveMessage(string);
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);
    
    DestoryTransactionManager(&manager);
}




