#include <stdio.h>
#include <assert.h>

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
#include "TransactionNotifyInterface.h"
#include "utils/list/include/list.h"

#define TRANSACTION_ACTIONS_MAX 5

struct Transaction {
    struct TransactionOwnerInterface *owner;
    enum TransactionState state;
    struct Message *request;
    t_list *responses;
    struct TransactionManagerInterface *manager;
    int timerEFiredCount;
    int event;
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

static TransactionTimerAdder TimerAdder;
void RunFSM(struct Transaction *t, enum TransactionEvent event);

void TransactionSetTimer(TransactionTimerAdder adder)
{
    TimerAdder = adder;
}

void TransactionAddResponse(struct Transaction *t, struct Message *message)
{
    put_in_list(&t->responses, message);
}

void TransactionSetManagerInterface(struct Transaction *t, struct TransactionManagerInterface *manager)
{
    t->manager = manager;
}

enum TransactionState TransactionGetState(struct Transaction *t)
{
    return t->state;
}

struct Message *TransactionGetLatestResponse(struct Transaction *t)
{
    int length = get_list_len(t->responses);
    return get_data_at(t->responses, length - 1);
}

void TimerKCallBack(void *t)
{
    RunFSM(t, TRANSACTION_EVENT_TIMER_K_FIRED);
}

void TimerFCallback(void *t)
{
    RunFSM(t, TRANSACTION_EVENT_TIMER_F_FIRED);
}

void TimerECallback(void *t)
{
    RunFSM(t, TRANSACTION_EVENT_TIMER_E_FIRED);
}

int ResetTimerEFiredCount(struct Transaction *t)
{
    t->timerEFiredCount = 0;
    return 0;
}

int IncTimerEFiredCount(struct Transaction *t)
{
    if (T1<<t->timerEFiredCount <= T4)
        t->timerEFiredCount ++;

    return 0;
}

int AddTimerE(struct Transaction *t)
{    
    int interval = T1<<t->timerEFiredCount;
    
    if (interval > T4)
        interval = T4;
    if (TimerAdder)
        TimerAdder(t, interval, TimerECallback);

    return 0;
}

int AddTimerK(struct Transaction *t)
{
    if (TimerAdder)
        TimerAdder(t, T4, TimerKCallBack);

    return 0;
}

int NotifyOwner(struct Transaction *t)
{
    if (t && t->owner) {
        t->owner->onEvent(t);
    }
    return 0;
}

int SendRequestMessage(struct Transaction *t)
{
    char s[MAX_MESSAGE_LENGTH] = {0};
    Message2String(s, t->request);

    if (SendMessage(s) < 0) {
        RunFSM(t, TRANSACTION_EVENT_TRANSPORT_ERROR);
        return -1;
    }

    return 0;
}

struct Message *TransactionGetRequest(struct Transaction *t)
{
    return t->request;
}

enum TransactionEvent TransactionGetCurrentEvent(struct Transaction *t)
{
    return t->event;
}

struct TransactionOwnerInterface *TransactionGetOwner(struct Transaction *t)
{
    return t->owner;
}

struct Transaction *CallocTransaction(struct Message *request)
{
    struct Transaction *t;

    t = calloc(1, sizeof (struct Transaction));
    t->state = TRANSACTION_STATE_TRYING;
    t->request = request;

    return t;
}

struct Transaction *CreateTransaction(struct Message *request, struct TransactionOwnerInterface *owner)
{
    struct Transaction *t;

    t = CallocTransaction(request);
    if (SendRequestMessage(t) < 0) {
        DestoryTransaction(&t);
        return NULL;
    }

    t->owner = owner;
    if (TimerAdder != NULL) {
        TimerAdder(t, T1, TimerECallback);    
        TimerAdder(t, 64*T1, TimerFCallback);
    }

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

struct FSM_STATE ClientNonInviteTransactionFSM[TRANSACTION_STATE_MAX] = {
    {TRANSACTION_STATE_TRYING,{
            {TRANSACTION_EVENT_200OK, TRANSACTION_STATE_COMPLETED,{AddTimerK, NotifyOwner}},
            {TRANSACTION_EVENT_100TRYING,TRANSACTION_STATE_PROCEEDING,{ResetTimerEFiredCount}},
            {TRANSACTION_EVENT_TIMER_E_FIRED,TRANSACTION_STATE_TRYING,{
                    SendRequestMessage,
                    IncTimerEFiredCount,
                    AddTimerE}},
            {TRANSACTION_EVENT_TIMER_F_FIRED,TRANSACTION_STATE_TERMINATED,{NULL}},
            {TRANSACTION_EVENT_TRANSPORT_ERROR, TRANSACTION_STATE_TERMINATED,{NULL}},
            {-1}}},
    {TRANSACTION_STATE_PROCEEDING,{
            {TRANSACTION_EVENT_200OK, TRANSACTION_STATE_COMPLETED,{AddTimerK}},
            {TRANSACTION_EVENT_100TRYING,TRANSACTION_STATE_PROCEEDING,{NULL}},
            {TRANSACTION_EVENT_TIMER_E_FIRED,TRANSACTION_STATE_PROCEEDING,{
                    SendRequestMessage,
                    IncTimerEFiredCount,
                    AddTimerE}},
            {TRANSACTION_EVENT_TRANSPORT_ERROR, TRANSACTION_STATE_TERMINATED,{NULL}},
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

    for (; ClientNonInviteTransactionFSM[i].currState != -1; i ++) {
        if (t->state == ClientNonInviteTransactionFSM[i].currState)
            fsmState = &ClientNonInviteTransactionFSM[i];
    }

    return fsmState;
}

void InvokeActions(struct Transaction *t, struct FSM_STATE_ENTRY *e)
{
    int j;

    for (j = 0; j < TRANSACTION_ACTIONS_MAX; j++) {
        if(e->actions[j] != NULL) 
            if (e->actions[j](t) < 0) break;
    }
}

void RunFSM(struct Transaction *t, enum TransactionEvent event)
{
    int i = 0;
    struct FSM_STATE_ENTRY *entrys = NULL;
    struct FSM_STATE *fsmState = LocateFSMState(t);
    if (fsmState == NULL) return;

    assert(t != NULL);
    
    entrys = fsmState->entrys;
    for ( i = 0; entrys[i].event != -1; i++) {
        if (entrys[i].event == event) {
            t->event = event;
            t->state = entrys[i].nextState;
            InvokeActions(t, &entrys[i]);
            break;
        }
    }

    if (TransactionGetState(t) == TRANSACTION_STATE_TERMINATED)
        if (t != NULL && t->manager != NULL)
            ((struct Transaction *)t)->manager->die(t);

}
