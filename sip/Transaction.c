#include <stdio.h>

#include "Bool.h"
#include "Timer.h"
#include "Transaction.h"
#include "MessageBuilder.h"
#include "Messages.h"
#include "MessageTransport.h"
#include "StatusLine.h"
#include "Header.h"
#include "ViaHeader.h"
#include "CSeqHeader.h"
#include "utils/list/include/list.h"

#define TRANSACTION_ACTIONS_MAX 5

struct Transaction {
    enum TransactionState state;
    struct Message *request;
    t_list *responses;
    int timerEFiredCount;
};

struct FSM_STATE_ENTRY {
    enum TransactionEvent event;
    enum TransactionState nextState;
    TransactionAction actions[TRANSACTION_ACTIONS_MAX + 1];
};

struct FSM_STATE {
    enum TransactionState currState;
    struct FSM_STATE_ENTRY entrys[TRANSACTION_EVENT_MAX + 1];
};

struct Transaction  *Transaction;
static TransactionTimerAdder TimerAdder;

void RunFSM(struct Transaction *t, enum TransactionEvent event);

void TransactionSetTimer(TransactionTimerAdder adder)
{
    TimerAdder = adder;
}

enum TransactionState TransactionGetState(struct Transaction *t)
{
    return t->state;
}

BOOL MatchResponse(struct Message *request, struct Message *response)
{
    return ViaBranchMatched((struct ViaHeader *)MessageGetHeader(HEADER_NAME_VIA, request),
                            (struct ViaHeader *)MessageGetHeader(HEADER_NAME_VIA, response))
        && CSeqHeaderMethodMatched((struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, request),
                                   (struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, response));
}

void TimerKCallBack(void *t)
{
    RunFSM(Transaction, TRANSACTION_EVENT_TIMER_K_FIRED);
}

void TimerFCallback(void *t)
{
    RunFSM(Transaction, TRANSACTION_EVENT_TIMER_F_FIRED);
}

void TimerECallback(void *t)
{
    RunFSM(Transaction, TRANSACTION_EVENT_TIMER_E_FIRED);
}

void ResetTimerEFiredCount(struct Transaction *t)
{
    t->timerEFiredCount = 0;
}

void IncTimerEFiredCount(struct Transaction *t)
{
    if (T1<<t->timerEFiredCount <= T4)
        t->timerEFiredCount ++;
}

void AddTimerE(struct Transaction *t)
{    
    int interval = T1<<t->timerEFiredCount;
    
    if (interval > T4)
        interval = T4;
    
    TimerAdder(Transaction, interval, TimerECallback);
}

void AddTimerK(struct Transaction *t)
{
    TimerAdder(Transaction, T4, TimerKCallBack);
}

void SendRequestMessage(struct Transaction *t)
{
    char s[MAX_MESSAGE_LENGTH] = {0};
    Message2String(s, t->request);
    SendMessage(s);
}

struct Transaction *CallocTransaction(struct Message *request)
{
    struct Transaction *t;

    t = calloc(1, sizeof (struct Transaction));
    t->state = TRANSACTION_STATE_TRYING;
    t->request = request;

    return t;
}

struct Transaction *CreateTransaction(struct Message *request)
{
    struct Transaction *t;

    t = CallocTransaction(request);
    SendRequestMessage(t);
    TimerAdder(t, T1, TimerECallback);
    TimerAdder(t, 64*T1, TimerFCallback);

    Transaction = t;

    return t;
}

void DestoryResponseMessage(struct Transaction *t)
{
    int length = get_list_len(t->responses);
    int i = 0;

    for (; i < length; i ++) {
        struct Message *message = get_data_at(t->responses, i);
        DestoryMessage(&message);
    }
}

void DestoryTransaction(struct Transaction **t)
{
    if ( *t != NULL) {
        DestoryMessage(&(*t)->request);
        DestoryResponseMessage(*t);
        free(*t);
        *t = NULL;
    }
}

struct FSM_STATE TransactionFSM[TRANSACTION_STATE_MAX] = {
    {TRANSACTION_STATE_TRYING,{
            {TRANSACTION_EVENT_200OK, TRANSACTION_STATE_COMPLETED,{AddTimerK}},
            {TRANSACTION_EVENT_100TRYING,TRANSACTION_STATE_PROCEEDING,{ResetTimerEFiredCount}},
            {TRANSACTION_EVENT_TIMER_E_FIRED,TRANSACTION_STATE_TRYING,{
                    IncTimerEFiredCount,
                    AddTimerE,
                    SendRequestMessage}},
            {TRANSACTION_EVENT_TIMER_F_FIRED,TRANSACTION_STATE_TERMINATED,{NULL}},
            {-1}}},
    {TRANSACTION_STATE_PROCEEDING,{
            {TRANSACTION_EVENT_200OK, TRANSACTION_STATE_COMPLETED,{AddTimerK}},
            {TRANSACTION_EVENT_100TRYING,TRANSACTION_STATE_PROCEEDING,{NULL}},
            {TRANSACTION_EVENT_TIMER_E_FIRED,TRANSACTION_STATE_PROCEEDING,{
                    IncTimerEFiredCount,
                    AddTimerE,
                    SendRequestMessage}},
            {-1}}},
    {TRANSACTION_STATE_COMPLETED,{
            {TRANSACTION_EVENT_TIMER_K_FIRED, TRANSACTION_STATE_TERMINATED,{NULL}},
            {-1}}},
    {-1},
};

struct FSM_STATE *LocateFSMState(struct Transaction *t)
{
    int i = 0;
    struct FSM_STATE *fsmState = NULL;

    for (; TransactionFSM[i].currState != -1; i ++) {
        if (t->state == TransactionFSM[i].currState)
            fsmState = &TransactionFSM[i];
    }

    return fsmState;
}

void InvokeActions(struct Transaction *t, struct FSM_STATE_ENTRY *e)
{
    int j;

    for (j = 0; j < TRANSACTION_ACTIONS_MAX; j++) {
        if(e->actions[j] != NULL) e->actions[j](t);
    }
}

void RunFSM(struct Transaction *t, enum TransactionEvent event)
{
    int i = 0;
    struct FSM_STATE_ENTRY *entrys = NULL;
    struct FSM_STATE *fsmState = LocateFSMState(t);
    if (fsmState == NULL) return;

    entrys = fsmState->entrys;
    for ( i = 0; entrys[i].event != -1; i++) {
        if (entrys[i].event == event) {
            t->state = entrys[i].nextState;
            InvokeActions(t, &entrys[i]);
        }
    }
}

int TransactionHandleMessage(char *string)
{
    struct Message *message = CreateMessage();
    struct StatusLine *status = NULL;
    int statusCode = 0;

    ParseMessage(string, message);
    status = MessageGetStatus(message);
    statusCode = StatusLineGetStatusCode(status);

    if (MatchResponse(Transaction->request, message)){
        if (statusCode == 200) {
            RunFSM(Transaction, TRANSACTION_EVENT_200OK);
        }
        else if (statusCode == 100) {
            RunFSM(Transaction, TRANSACTION_EVENT_100TRYING);
        }
    }

    put_in_list(&Transaction->responses, message);
    return 0;
}
