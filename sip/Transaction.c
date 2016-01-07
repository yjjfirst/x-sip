#include <stdio.h>
#include <assert.h>

#include "Bool.h"
#include "Transaction.h"
#include "MessageBuilder.h"
#include "Messages.h"
#include "MessageTransport.h"
#include "StatusLine.h"
#include "Header.h"
#include "ViaHeader.h"
#include "CSeqHeader.h"
#include "TransactionNotifiers.h"
#include "utils/list/include/list.h"

#define TRANSACTION_ACTIONS_MAX 10

struct Transaction {
    struct TransactionOwner *owner;
    enum TransactionState state;
    struct FSM *fsm;
    struct Message *request;
    t_list *responses;
    struct TransactionNotifiers *notifiers;
    int retransmits;
    int event;
    enum TransactionType type;
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

struct FSM {
    struct FSM_STATE *fsmStates[TRANSACTION_STATE_MAX];
};

struct FSM ClientTransactionFsm;

void TransactionAddResponse(struct Transaction *t, struct Message *message)
{
    put_in_list(&t->responses, message);
}

void TransactionSetNotifiers(struct Transaction *t, struct TransactionNotifiers *notifiers)
{
    t->notifiers = notifiers;
}

enum TransactionState TransactionGetState(struct Transaction *t)
{
    assert(t != NULL);
    return t->state;
}

struct Message *TransactionGetLatestResponse(struct Transaction *t)
{
    int length = get_list_len(t->responses);
    return get_data_at(t->responses, length - 1);
}

void WaitForResponseTimerCallBack(void *t)
{
    RunFsm(t, TRANSACTION_EVENT_WAIT_FOR_RESPONSE_TIMER_FIRED);
}

void TimeoutTimerCallback(void *t)
{
    RunFsm(t, TRANSACTION_EVENT_TIMEOUT_TIMER_FIRED);
}

void RetransmitTimerCallback(void *t)
{
    RunFsm(t, TRANSACTION_EVENT_RETRANSMIT_TIMER_FIRED);
}

int ResetRetransmitTimer(struct Transaction *t)
{
    t->retransmits = 0;
    return 0;
}

int IncRetransmit(struct Transaction *t)
{
    if (T1<<t->retransmits <= T4)
        t->retransmits ++;

    return 0;
}

int AddRetransmitTimer(struct Transaction *t)
{    
    int interval = T1<<t->retransmits;
    
    if (interval > T4)
        interval = T4;

    AddTimer(t, interval, RetransmitTimerCallback);

    return 0;
}

int AddWaitForResponseTimer(struct Transaction *t)
{
    AddTimer(t, T4, WaitForResponseTimerCallBack);

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

    assert(t != NULL);
    assert(t->request != NULL);

    Message2String(s, t->request);

    if (SendOutMessage(s) < 0) {
        RunFsm(t, TRANSACTION_EVENT_TRANSPORT_ERROR);
        return -1;
    }
    
    return 0;
}

struct Message *TransactionGetRequest(struct Transaction *t)
{
    assert (t != NULL);
    return t->request;
}

enum TransactionType TransactionGetType(struct Transaction *t)
{
    return t->type;
}

enum TransactionEvent TransactionGetCurrentEvent(struct Transaction *t)
{
    return t->event;
}

struct TransactionOwner *TransactionGetOwner(struct Transaction *t)
{
    return t->owner;
}

struct Transaction *CallocTransaction()
{
    return  calloc(1, sizeof (struct Transaction));
}

struct Transaction *CreateClientTransaction(struct Message *request, struct TransactionOwner *owner)
{
    struct Transaction *t = CallocTransaction();
    struct RequestLine *rl = MessageGetRequestLine(request);
    
    if(RequestLineGetMethod(rl) == SIP_METHOD_INVITE) {
        t->state = TRANSACTION_STATE_CALLING;
        t->type = TRANSACTION_TYPE_CLIENT_INVITE;
    } else {
        t->state = TRANSACTION_STATE_TRYING;
        t->type = TRANSACTION_TYPE_CLIENT_NON_INVITE;
    }
    
    t->request = request;
    t->owner = owner;
    t->fsm = &ClientTransactionFsm;

    if (SendRequestMessage(t) < 0) {
        DestoryTransaction(&t);
        return NULL;
    }
    AddTimer(t, T1, RetransmitTimerCallback);    
    AddTimer(t, 64*T1, TimeoutTimerCallback);

    return t;
}

struct Transaction *CreateServerTransaction(struct Message *request, struct TransactionOwner *owner)
{
    struct Transaction *t = CallocTransaction();
    t->state = TRANSACTION_STATE_PROCEEDING;
    t->request = request;
    t->owner = owner;

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

struct FSM_STATE TryingState = {
    TRANSACTION_STATE_TRYING,
    {
        {TRANSACTION_EVENT_200OK, TRANSACTION_STATE_COMPLETED, {AddWaitForResponseTimer, NotifyOwner}},
        {TRANSACTION_EVENT_100TRYING,TRANSACTION_STATE_PROCEEDING, {ResetRetransmitTimer}},
        {TRANSACTION_EVENT_RETRANSMIT_TIMER_FIRED,TRANSACTION_STATE_TRYING, {
                SendRequestMessage,
                IncRetransmit,
                AddRetransmitTimer}},
        {TRANSACTION_EVENT_TIMEOUT_TIMER_FIRED,TRANSACTION_STATE_TERMINATED,{NULL}},
        {TRANSACTION_EVENT_TRANSPORT_ERROR, TRANSACTION_STATE_TERMINATED,{NULL}},
        {TRANSACTION_EVENT_MAX},
    }
}; 

struct FSM_STATE CallingState = {
    TRANSACTION_STATE_CALLING,
    {
        {TRANSACTION_EVENT_200OK, TRANSACTION_STATE_TERMINATED,{AddWaitForResponseTimer, NotifyOwner}},
        {TRANSACTION_EVENT_100TRYING,TRANSACTION_STATE_PROCEEDING,{ResetRetransmitTimer}},
        {TRANSACTION_EVENT_RETRANSMIT_TIMER_FIRED,TRANSACTION_STATE_TRYING,{
                SendRequestMessage,
                IncRetransmit,
                AddRetransmitTimer}},
        {TRANSACTION_EVENT_TIMEOUT_TIMER_FIRED,TRANSACTION_STATE_TERMINATED,{NULL}},
        {TRANSACTION_EVENT_TRANSPORT_ERROR, TRANSACTION_STATE_TERMINATED,{NULL}},
        {TRANSACTION_EVENT_MAX}
    }
};

struct FSM_STATE ProceedingState = {
    TRANSACTION_STATE_PROCEEDING,
    {
        {TRANSACTION_EVENT_200OK, TRANSACTION_STATE_COMPLETED,{AddWaitForResponseTimer}},
        {TRANSACTION_EVENT_100TRYING,TRANSACTION_STATE_PROCEEDING,{NULL}},
        {TRANSACTION_EVENT_180RINGING, TRANSACTION_STATE_PROCEEDING,{NULL}},
        {TRANSACTION_EVENT_RETRANSMIT_TIMER_FIRED,TRANSACTION_STATE_PROCEEDING,{
                SendRequestMessage,
                IncRetransmit,
                AddRetransmitTimer}},
        {TRANSACTION_EVENT_TRANSPORT_ERROR, TRANSACTION_STATE_TERMINATED,{NULL}},
        {TRANSACTION_EVENT_MAX}
    }
};

struct FSM_STATE CompletedState = {
    TRANSACTION_STATE_COMPLETED,
    {
        {TRANSACTION_EVENT_WAIT_FOR_RESPONSE_TIMER_FIRED, TRANSACTION_STATE_TERMINATED,{NULL}},
        {TRANSACTION_EVENT_MAX}
    }
};

struct FSM ClientTransactionFsm = {
    {
        &TryingState,
        &CallingState,
        &ProceedingState,
        &CompletedState,
        NULL,
    }
};

struct FSM_STATE *LocateFsmState(struct Transaction *t)
{
    int i = 0;
    struct FSM_STATE *fsmState = NULL;
    struct FSM *fsm = t->fsm;

    assert (t != NULL);
    assert (t->fsm != NULL);

    for (; fsm->fsmStates[i] != NULL; i ++) {
        if (t->state == fsm->fsmStates[i]->currState)
            fsmState = fsm->fsmStates[i];
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

void RunFsm(struct Transaction *t, enum TransactionEvent event)
{
    int i = 0;
    struct FSM_STATE_ENTRY *entrys = NULL;
    struct FSM_STATE *fsmState = LocateFsmState(t);

    assert(t != NULL);

    if (fsmState == NULL) return;
    
    entrys = fsmState->entrys;
    for ( i = 0; entrys[i].event != TRANSACTION_EVENT_MAX; i++) {
        if (entrys[i].event == event) {
            t->event = event;
            t->state = entrys[i].nextState;
            InvokeActions(t, &entrys[i]);
            break;
        }
    }

    if (TransactionGetState(t) == TRANSACTION_STATE_TERMINATED)
        if (t != NULL && t->notifiers != NULL)
            ((struct Transaction *)t)->notifiers->die(t);

}
