#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include <stdio.h>
#include <string.h>

#include "Timer.h"
#include "Transaction.h"
#include "MessageBuilder.h"
#include "Messages.h"
#include "MessageTransport.h"
#include "Header.h"
#include "Parameter.h"
#include "ViaHeader.h"
#include "Method.h"
}

enum Response {
    OK200,
    RINGING180,
};

char OKMessage[] = "\
SIP/2.0 200 OK\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5060;branch=z9hG4bK1491280923;received=192.168.10.1;rport=5060\r\n\
From: <sip:88001@192.168.10.62>;tag=1225432999\r\n\
To: <sip:88001@192.168.10.62>;tag=as1d07559a\r\n\
Call-ID: 1222971951\r\n\
CSeq: 1 REGISTER\r\n\
Expires:3600\r\n\
Contact: <sip:88001@192.168.10.1;line=f2fd53ebfa7728f>;expires=3600\r\n\
Content-Length: 0\r\n";

char TryingMessage[] = "\
SIP/2.0 100 Trying\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5060;branch=z9hG4bK1491280923;received=192.168.10.1;rport=5060\r\n\
From: <sip:88001@192.168.10.62>;tag=1225432999\r\n\
To: <sip:88001@192.168.10.62>;tag=as1d07559a\r\n\
Call-ID: 1222971951\r\n\
CSeq: 1 REGISTER\r\n\
Expires:3600\r\n\
Contact: <sip:88001@192.168.10.1;line=f2fd53ebfa7728f>;expires=3600\r\n\
Content-Length: 0\r\n";

enum Response Response;
static TimerCallback TimerCallbackFunc;
static TimerCallback TimerFCallbackFunc;
static TimerCallback TimerKCallbackFunc;
struct Transaction *Transaction;

int TransactionReceiveMessageMock(char *message)
{
    if (Response == RINGING180)
        strcpy(message, TryingMessage);
    else
        strcpy(message, OKMessage);
    return 0;
}

int TransactionSendMessageMock(char *message)
{
    return 0;
}

void AddTimer(void *p, int ms, TimerCallback onTime)
{
    mock().actualCall("AddTimer").withIntParameter("ms", ms);
    if (TimerCallbackFunc == NULL)
        TimerCallbackFunc = onTime;
    else if (TimerFCallbackFunc == NULL)
        TimerFCallbackFunc = onTime;
    else 
        TimerKCallbackFunc = onTime;
}

TEST_GROUP(TransactionTestGroup)
{
    struct Message *m;
    struct Transaction *t;
    enum TransactionState s;

    void setup()
    {
        mock().expectOneCall("AddTimer").withIntParameter("ms", T1);
        mock().expectOneCall("AddTimer").withIntParameter("ms", 64*T1);

        AddMessageTransporter((char *)"TRANS", TransactionSendMessageMock, TransactionReceiveMessageMock);
        InitReceiveMessageCallback(TransactionHandleMessage);
        TransactionSetTimerAdder(AddTimer);

        m = BuildRegisterMessage();
        t = CreateTransaction(m);
        s = TransactionGetState(t);

        MessageAddViaParameter(m, (char *)"rport", (char *)"");
        MessageAddViaParameter(m, (char *)"branch", (char *)"z9hG4bK1491280923");
    }
    
    void teardown()
    {
        RemoveMessageTransporter((char *)"TRANS");
        DestoryTransaction(&t);
        mock().clear();
    }
};

TEST(TransactionTestGroup, TransactionInitTest)
{
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);
}

TEST(TransactionTestGroup, Receive1xxTest)
{
    char string[MAX_MESSAGE_LENGTH] = {0};

    Response = RINGING180;

    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);
    
    
    ReceiveMessage(string); 
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, s);

    ReceiveMessage(string);
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, s);

    Response = OK200;
    mock().expectOneCall("AddTimer").withIntParameter("ms", T4);    
    ReceiveMessage(string);
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, s);


}

TEST(TransactionTestGroup, Receive2xxTest)
{
    char string[MAX_MESSAGE_LENGTH] = {0};

    Response = OK200;
    mock().expectOneCall("AddTimer").withIntParameter("ms", T4);    
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);


    ReceiveMessage(string); 
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, s);
}

TEST(TransactionTestGroup, BranchNonMatchTest)
{
    char string[MAX_MESSAGE_LENGTH] = {0};

    MessageAddViaParameter(m, (char *)"branch", (char *)"z9hG4bK1491280924");
    
    ReceiveMessage(string);
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);
}

TEST(TransactionTestGroup, CSeqMethodNonMatchTest)
{
    char string[MAX_MESSAGE_LENGTH] = {0};

    MessageSetCSeqMethod(m, (char *)SIP_METHOD_NAME_INVITE);
    ReceiveMessage(string);
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);
}

TEST(TransactionTestGroup, TryingTimeETest)
{
    mock().expectOneCall("AddTimer").withIntParameter("ms", T1);    

    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);
    TimerCallbackFunc(Transaction);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);

    mock().checkExpectations();
    
}

TEST(TransactionTestGroup, ProceedingTimeETest)
{
    char string[MAX_MESSAGE_LENGTH] = {0};

    Response = RINGING180;
    
    ReceiveMessage(string); 
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, s);

    mock().expectOneCall("AddTimer").withIntParameter("ms", T1);    
    TimerCallbackFunc(Transaction);
    CHECK_EQUAL(TRANSACTION_STATE_PROCEEDING, s);
    mock().checkExpectations();
    
}

TEST(TransactionTestGroup, TimerFTest)
{
    TimerFCallbackFunc(Transaction);
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_TERMINATED, s);

}

TEST(TransactionTestGroup, TimerKTest)
{
    char string[MAX_MESSAGE_LENGTH] = {0};

    Response = OK200;
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);
    mock().expectOneCall("AddTimer").withIntParameter("ms", T4);    

    ReceiveMessage(string); 
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, s);

    TimerKCallbackFunc(Transaction);
    s = TransactionGetState(t);
    CHECK_EQUAL(TRANSACTION_STATE_TERMINATED, s);
}
