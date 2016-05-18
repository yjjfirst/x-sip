#include <stdio.h>
#include <assert.h>

#include "Bool.h"
#include "Transaction.h"
#include "TransactionId.h"
#include "TransactionManager.h"
#include "MessageBuilder.h"
#include "Messages.h"
#include "MessageTransport.h"
#include "StatusLine.h"
#include "RequestLine.h"
#include "Header.h"
#include "ViaHeader.h"
#include "CSeqHeader.h"
#include "TransactionNotifiers.h"
#include "utils/list/include/list.h"

#define TRANSACTION_ACTIONS_MAX 10

struct Transaction {
    struct TransactionUserNotifiers *user;
    enum TransactionState state;
    struct Fsm *fsm;
    struct Message *request;
    t_list *responses;
    struct TransactionManagerNotifiers *notifiers;
    int retransmits;
    int curEvent;
    enum TransactionType type;
    struct TransactionId *id;
};

struct FsmStateEventEntry {
    enum TransactionEvent event;
    enum TransactionState nextState;
    TransactionAction actions[TRANSACTION_ACTIONS_MAX + 1];
};

struct FsmState {
    enum TransactionState currState;
    struct FsmStateEventEntry entrys[TRANSACTION_EVENT_MAX + 1];
};

struct Fsm {
    struct FsmState *fsmStates[TRANSACTION_STATE_MAX];
};

struct Fsm ClientNonInviteTransactionFsm;
struct Fsm ClientInviteTransactionFsm;
struct Fsm ServerInviteTransactionFsm;
struct Fsm ServerNonInviteTransactionFsm;

void TransactionAddResponse(struct Transaction *t, struct Message *message)
{
    put_in_list(&t->responses, message);
}

void TransactionSetNotifiers(struct Transaction *t, struct TransactionManagerNotifiers *notifiers)
{
    t->notifiers = notifiers;
}

enum TransactionState TransactionGetState(struct Transaction *t)
{
    assert(t != NULL);
    return t->state;
}

struct TransactionId *TransactionGetId(struct Transaction *t)
{
    assert(t != NULL);
    return t->id;
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

void WaitForRequestRetransmitTimerCallback(void *t)
{
    RunFsm(t, TRANSACTION_EVENT_WAIT_REQUEST_RETRANSMITS_TIMER_FIRED);
}

int ResetRetransmitTimer(struct Transaction *t)
{
    t->retransmits = 0;
    return 0;
}

int IncRetransmit(struct Transaction *t)
{
    if ( (INITIAL_REQUEST_RETRANSMIT_INTERVAL<<t->retransmits) <= MAXIMUM_RETRANSMIT_INTERVAL)
        t->retransmits ++;

    return 0;
}

int CalculateRetransmitInterval(struct Transaction *t)
{
    int interval = INITIAL_REQUEST_RETRANSMIT_INTERVAL<<t->retransmits;
    if (interval > MAXIMUM_RETRANSMIT_INTERVAL) interval = MAXIMUM_RETRANSMIT_INTERVAL;
    return interval;
}

int ClientInviteAddRetransmitTimer(struct Transaction *t)
{    
    AddTimer(t, CalculateRetransmitInterval(t), RetransmitTimerCallback);
    return 0;
}

int ClientNonInviteAddRetransmitTimer(struct Transaction *t)
{    
    int interval = INITIAL_REQUEST_RETRANSMIT_INTERVAL;
    
    if (t->state == TRANSACTION_STATE_TRYING){
        interval = CalculateRetransmitInterval(t);
    } else if (t->state == TRANSACTION_STATE_PROCEEDING)
        interval = MAXIMUM_RETRANSMIT_INTERVAL;
    
    AddTimer(t, interval, RetransmitTimerCallback);
    return 0;
}

int AddTimeoutTimer(struct Transaction *t)
{
    AddTimer(t, TRANSACTION_TIMEOUT_INTERVAL, TimeoutTimerCallback);
    return 0;
}

int AddWaitForResponseTimer(struct Transaction *t)
{
    AddTimer(t, WAIT_TIME_FOR_ACK_RETRANSMITS, WaitForResponseTimerCallBack);
    return 0;
}

int AddServerNonInviteWaitRequestRetransmitTimer(struct Transaction *t)
{
    AddTimer(t, WAIT_TIME_FOR_REQUEST_RETRANSMITS, WaitForRequestRetransmitTimerCallback);
    return 0;
}

int NotifyUser(struct Transaction *t)
{
    if (t && t->user) {
        t->user->onEvent(t);
    }
    return 0;
}

int TransactionSendMessage(struct Message *message)
{
    char s[MAX_MESSAGE_LENGTH] = {0};
    Message2String(s, message);

    return SendOutMessage(s);
}

int SendRequestMessage(struct Transaction *t)
{
    assert(t != NULL);
    assert(t->request != NULL);

    if (TransactionSendMessage(t->request) < 0) {
        RunFsm(t, TRANSACTION_EVENT_TRANSPORT_ERROR);
        return -1;
    }
    
    return 0;
}

int ResendLatestResponse(struct Transaction *t)
{
    if (TransactionSendMessage(TransactionGetLatestResponse(t)) < 0) {
        RunFsm(t, TRANSACTION_EVENT_TRANSPORT_ERROR);
        return -1;
    }
    return 0;
}

BOOL IfResponseMatchedTransaction(struct Transaction *t, struct Message *response)
{
    assert (t != NULL);
    assert (response != NULL);

    struct Message *request = TransactionGetRequest(t);
    return MessageViaHeaderBranchMatched(request, response) 
        && MessageCSeqHeaderMethodMatched(request, response);
}

BOOL RequestMethodMatched(struct Transaction *t, struct Message *request)
{
    struct RequestLine *origin = MessageGetRequestLine(t->request);
    struct RequestLine *new = MessageGetRequestLine(request);

    return RequestLineMethodMatched(origin, new);
}

BOOL IfRequestMatchTransaction(struct Transaction *t, struct Message *request)
{
    assert(t != NULL);
    assert(request != NULL);

    struct Message *origin = TransactionGetRequest(t);
    return MessageViaHeaderBranchMatched(origin, request)
        && MessageViaHeaderSendbyMatched(origin, request)
        && RequestMethodMatched(t, request);
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
    return t->curEvent;
}

struct TransactionUserNotifiers *TransactionGetUser(struct Transaction *t)
{
    return t->user;
}

struct Transaction *CallocTransaction(struct Message *request, struct TransactionUserNotifiers *user)
{
    struct Transaction *t = calloc(1, sizeof (struct Transaction));

    t->id = CreateTransactionId();
    t->request = request;
    t->user = user;
    TransactionIdExtract(t->id, request);

    return t;
}

void ResponseWith301(struct Transaction *t)
{
    struct Message *moved = Build301Message(t->request);

    TransactionAddResponse(t, moved);
    if (TransactionSendMessage(moved) < 0) {
        RunFsm(t, TRANSACTION_EVENT_TRANSPORT_ERROR);
        return;
    }

    RunFsm(t, TRANSACTION_EVENT_301MOVED_SENT);
}

void ResponseWith200OK(struct Transaction *t)
{
    struct Message *ok = Build200OKMessage(t->request);
    TransactionAddResponse(t, ok);
    TransactionSendMessage(ok);

    RunFsm(t, TRANSACTION_EVENT_200OK_SENT);
}

void ResponseWith180Ringing(struct Transaction *t)
{
    struct Message *ringing = BuildRingingMessage(t->request);

    TransactionAddResponse(t, ringing);
    if (TransactionSendMessage(ringing) < 0) {
        RunFsm(t, TRANSACTION_EVENT_TRANSPORT_ERROR);
        return;
    }

    if (TransactionGetType(t) == TRANSACTION_TYPE_SERVER_NON_INVITE)
        RunFsm(t, TRANSACTION_EVENT_1XX_SENT);
}

int SendAckRequest(struct Transaction *t)
{
    struct Message *ack=BuildAckMessageWithinClientTransaction(t->request);

    TransactionAddResponse(t, ack);
    if (TransactionSendMessage(ack) < 0) {
        RunFsm(t, TRANSACTION_EVENT_TRANSPORT_ERROR);
        return -1;
    }
    return 0;
}

void ReceiveAckRequest(struct Transaction *t)
{
    RunFsm(t, TRANSACTION_EVENT_ACK_RECEIVED);
}

void Receive3xxResponse(struct Transaction *t)
{
    RunFsm(t, TRANSACTION_EVENT_3XX_RECEIVED);
}

void ReceiveDupRequest(struct Transaction *t, struct Message *message)
{
    ResendLatestResponse(t);
}

struct Transaction *CreateClientInviteTransaction(struct Message *request, struct TransactionUserNotifiers *user)
{
    struct Transaction *t = CallocTransaction(request, user);
    
    t->state = TRANSACTION_STATE_CALLING;
    t->type = TRANSACTION_TYPE_CLIENT_INVITE;
    t->fsm = &ClientInviteTransactionFsm;

    if (SendRequestMessage(t) < 0) {
        DestoryTransaction(&t);
        return NULL;
    }

    ClientNonInviteAddRetransmitTimer(t);
    AddTimeoutTimer(t);

    return t;
}

struct Transaction *CreateClientNonInviteTransaction(struct Message *request, struct TransactionUserNotifiers *user)
{
    struct Transaction *t = CallocTransaction(request, user);

    t->state = TRANSACTION_STATE_TRYING;
    t->type = TRANSACTION_TYPE_CLIENT_NON_INVITE;
    t->fsm = &ClientNonInviteTransactionFsm;

    if (SendRequestMessage(t) < 0) {
        DestoryTransaction(&t);
        return NULL;
    }

    ClientNonInviteAddRetransmitTimer(t);
    AddTimeoutTimer(t);

    return t;
}

struct Transaction *CreateServerInviteTransaction(struct Message *request, struct TransactionUserNotifiers *user)
{
    struct Transaction *t = CallocTransaction(request, user);
    struct Message *trying = BuildTryingMessage(t->request);

    t->state = TRANSACTION_STATE_PROCEEDING;
    t->fsm = &ServerInviteTransactionFsm;
 
    TransactionAddResponse(t, trying);

    if (TransactionSendMessage(trying) < 0) {
        RunFsm(t, TRANSACTION_EVENT_TRANSPORT_ERROR);
        DestoryTransaction(&t);
        return NULL;
    }

    return t;
}

struct Transaction *CreateServerNonInviteTransaction(struct Message *request, struct TransactionUserNotifiers *user)
{
    struct Transaction *t = CallocTransaction(request, user);
    t->type = TRANSACTION_TYPE_SERVER_NON_INVITE;
    t->fsm = &ServerNonInviteTransactionFsm;

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
        DestoryTransactionId(&(*t)->id);
        free(*t);
        *t = NULL;
    }
}

struct FsmState ClientNonInviteTryingState = {
    TRANSACTION_STATE_TRYING,
    {
        {TRANSACTION_EVENT_200OK_RECEIVED, TRANSACTION_STATE_COMPLETED, {AddWaitForResponseTimer, NotifyUser}},
        {TRANSACTION_EVENT_100TRYING_RECEIVED,TRANSACTION_STATE_PROCEEDING, {ResetRetransmitTimer}},
        {TRANSACTION_EVENT_RETRANSMIT_TIMER_FIRED,TRANSACTION_STATE_TRYING, {
                SendRequestMessage,
                IncRetransmit,
                ClientNonInviteAddRetransmitTimer}},
        {TRANSACTION_EVENT_TIMEOUT_TIMER_FIRED,TRANSACTION_STATE_TERMINATED,{NULL}},
        {TRANSACTION_EVENT_TRANSPORT_ERROR, TRANSACTION_STATE_TERMINATED,{NULL}},
        {TRANSACTION_EVENT_MAX},
    }
}; 

struct FsmState ClientNonInviteProceedingState = {
    TRANSACTION_STATE_PROCEEDING,
    {
        {TRANSACTION_EVENT_200OK_RECEIVED, TRANSACTION_STATE_COMPLETED,{AddWaitForResponseTimer}},
        {TRANSACTION_EVENT_100TRYING_RECEIVED,TRANSACTION_STATE_PROCEEDING},
        {TRANSACTION_EVENT_180RINGING_RECEIVED, TRANSACTION_STATE_PROCEEDING},
        {TRANSACTION_EVENT_RETRANSMIT_TIMER_FIRED,TRANSACTION_STATE_PROCEEDING,{
                SendRequestMessage,
                IncRetransmit,
                ClientNonInviteAddRetransmitTimer}},
        {TRANSACTION_EVENT_TRANSPORT_ERROR, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_EVENT_TIMEOUT_TIMER_FIRED, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_EVENT_MAX}
    }
};

struct FsmState ClientNonInviteCompletedState = {
    TRANSACTION_STATE_COMPLETED,
    {
        {TRANSACTION_EVENT_WAIT_FOR_RESPONSE_TIMER_FIRED, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_EVENT_MAX}
    }
};

struct Fsm ClientNonInviteTransactionFsm = {
    {
        &ClientNonInviteTryingState,
        &ClientNonInviteProceedingState,
        &ClientNonInviteCompletedState,
        NULL,
    }
};

struct FsmState ClientInviteCallingState = {
    TRANSACTION_STATE_CALLING,
    {
        {TRANSACTION_EVENT_200OK_RECEIVED, TRANSACTION_STATE_TERMINATED,{NotifyUser,}},
        {TRANSACTION_EVENT_100TRYING_RECEIVED,TRANSACTION_STATE_PROCEEDING,{ResetRetransmitTimer}},
        {TRANSACTION_EVENT_RETRANSMIT_TIMER_FIRED,TRANSACTION_STATE_CALLING,{
                SendRequestMessage,
                IncRetransmit,
                ClientInviteAddRetransmitTimer}},
        {TRANSACTION_EVENT_TIMEOUT_TIMER_FIRED,TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_EVENT_TRANSPORT_ERROR, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_EVENT_3XX_RECEIVED, TRANSACTION_STATE_COMPLETED, {AddWaitForResponseTimer}},
        {TRANSACTION_EVENT_MAX}
    }
};

struct FsmState ClientInviteProceedingState = {
    TRANSACTION_STATE_PROCEEDING,
    {
        {TRANSACTION_EVENT_3XX_RECEIVED, TRANSACTION_STATE_COMPLETED, {AddWaitForResponseTimer}},
        {TRANSACTION_EVENT_200OK_RECEIVED, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_EVENT_100TRYING_RECEIVED, TRANSACTION_STATE_PROCEEDING},
        {TRANSACTION_EVENT_MAX},
    }
};

struct FsmState ClientInviteCompletedState = {
    TRANSACTION_STATE_COMPLETED,
    {
        {TRANSACTION_EVENT_3XX_RECEIVED, TRANSACTION_STATE_COMPLETED, {SendAckRequest}},
        {TRANSACTION_EVENT_TRANSPORT_ERROR, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_EVENT_WAIT_FOR_RESPONSE_TIMER_FIRED, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_EVENT_MAX},
    }
};

struct Fsm ClientInviteTransactionFsm = {
    {
        &ClientInviteCallingState,
        &ClientInviteProceedingState,
        &ClientInviteCompletedState,
        NULL,
    }
};

struct FsmState ServerInviteProceedingState = {
    TRANSACTION_STATE_PROCEEDING,
    {
        {TRANSACTION_EVENT_INVITE_RECEIVED, TRANSACTION_STATE_PROCEEDING,{ResendLatestResponse}},
        {TRANSACTION_EVENT_TRANSPORT_ERROR, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_EVENT_200OK_SENT, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_EVENT_301MOVED_SENT, TRANSACTION_STATE_COMPLETED, {ClientNonInviteAddRetransmitTimer, AddTimeoutTimer}},        
        {TRANSACTION_EVENT_MAX}
    }
};

struct FsmState ServerInviteCompletedState = {
    TRANSACTION_STATE_COMPLETED,
    {
        {TRANSACTION_EVENT_INVITE_RECEIVED, TRANSACTION_STATE_COMPLETED, {ResendLatestResponse}},
        {TRANSACTION_EVENT_RETRANSMIT_TIMER_FIRED, TRANSACTION_STATE_COMPLETED, {ResendLatestResponse}},
        {TRANSACTION_EVENT_TIMEOUT_TIMER_FIRED, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_EVENT_TRANSPORT_ERROR, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_EVENT_ACK_RECEIVED, TRANSACTION_STATE_CONFIRMED, {AddWaitForResponseTimer}},
        {TRANSACTION_EVENT_MAX}
    }
};

struct FsmState ServerInviteConfirmedState = {
    TRANSACTION_STATE_CONFIRMED,
    {
        {TRANSACTION_EVENT_WAIT_FOR_RESPONSE_TIMER_FIRED, TRANSACTION_STATE_TERMINATED},
    }
};

struct Fsm ServerInviteTransactionFsm = {
    {
        &ServerInviteProceedingState,
        &ServerInviteCompletedState,
        &ServerInviteConfirmedState,
        NULL,
    }
};

struct FsmState ServerNonInviteTryingState = {
    TRANSACTION_STATE_TRYING,
    {
        {TRANSACTION_EVENT_1XX_SENT, TRANSACTION_STATE_PROCEEDING},
        {TRANSACTION_EVENT_200OK_SENT, TRANSACTION_STATE_COMPLETED, {
                AddServerNonInviteWaitRequestRetransmitTimer}},
        {TRANSACTION_EVENT_MAX}
    }
};

struct FsmState ServerNonInviteProceedingState = {
    TRANSACTION_STATE_PROCEEDING,
    {
        {TRANSACTION_EVENT_1XX_SENT, TRANSACTION_STATE_PROCEEDING},
        {TRANSACTION_EVENT_TRANSPORT_ERROR, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_EVENT_200OK_SENT, TRANSACTION_STATE_COMPLETED, {
                AddServerNonInviteWaitRequestRetransmitTimer }},
        {TRANSACTION_EVENT_MAX}
    }
};

struct FsmState ServerNonInviteCompletedState = {
    TRANSACTION_STATE_COMPLETED,
    {
        {TRANSACTION_EVENT_TRANSPORT_ERROR, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_EVENT_WAIT_REQUEST_RETRANSMITS_TIMER_FIRED, TRANSACTION_STATE_TERMINATED},
    }
};

struct Fsm ServerNonInviteTransactionFsm = {
    {
        &ServerNonInviteTryingState,
        &ServerNonInviteProceedingState,
        &ServerNonInviteCompletedState,
        NULL,
    }
};

void InvokeActions(struct Transaction *t, struct FsmStateEventEntry *e)
{
    int j;

    for (j = 0; j < TRANSACTION_ACTIONS_MAX; j++) {
        if(e->actions[j] != NULL) 
            if (e->actions[j](t) < 0) break;
    }
}

void TransactionTerminate(struct Transaction *t)
{
    if (TransactionGetState(t) == TRANSACTION_STATE_TERMINATED)
        if (t != NULL && t->notifiers != NULL)
            ((struct Transaction *)t)->notifiers->die(t);
}

void TransactionHandleEvent(struct Transaction *t, enum TransactionEvent event, struct FsmStateEventEntry *entry)
{

    t->curEvent = event;
    t->state = entry->nextState;
    InvokeActions(t, entry);

}

struct FsmState *LocateFsmState(struct Transaction *t)
{
    int i = 0;
    struct FsmState *fsmState = NULL;
    struct Fsm *fsm = t->fsm;

    assert (t != NULL);
    assert (t->fsm != NULL);

    for (; fsm->fsmStates[i] != NULL; i ++) {
        if (t->state == fsm->fsmStates[i]->currState)
            fsmState = fsm->fsmStates[i];
    }

    return fsmState;
}

struct FsmStateEventEntry *LocateEventEntry(struct Transaction *t, enum TransactionEvent event)
{
    int i = 0;
    struct FsmStateEventEntry *entrys = NULL;
    struct FsmState *fsmState = NULL;

    if ((fsmState = LocateFsmState(t)) == NULL) return NULL;

    entrys = fsmState->entrys;
    for ( i = 0; entrys[i].event != TRANSACTION_EVENT_MAX; i++) {
        if (entrys[i].event == event) {
            return &entrys[i];
        }
    }

    return NULL;
}

void RunFsm(struct Transaction *t, enum TransactionEvent event)
{
    assert(t != NULL);
    struct FsmStateEventEntry *entry = NULL;
    
    if ((entry = LocateEventEntry(t, event)) != NULL) {
        TransactionHandleEvent(t, event, entry);            
        TransactionTerminate(t);
        return;
    } 

    assert ("FSM event handle error" == 0);
}
