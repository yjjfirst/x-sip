#include <stdio.h>
#include <assert.h>

#include "Bool.h"
#include "Dialog.h"
#include "Transaction.h"
#include "TransactionId.h"
#include "TransactionManager.h"
#include "MessageBuilder.h"
#include "Messages.h"
#include "Transporter.h"
#include "StatusLine.h"
#include "RequestLine.h"
#include "Header.h"
#include "ViaHeader.h"
#include "CSeqHeader.h"
#include "utils/list/include/list.h"

#define TRANSACTION_ACTIONS_MAX 10

struct Transaction {
    struct TransactionUser *user;
    struct TransactionManager *manager;
    struct TransactionId *id;
    enum TransactionType type;

    enum TransactionState state;
    struct Fsm *fsm;
    MESSAGE *request;
    t_list *responses;
    int retransmits;
    int curEvent;

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

void TransactionAddResponse(struct Transaction *t, MESSAGE *message)
{
    put_in_list(&t->responses, message);
}

void TransactionSetObserver(struct Transaction *t, struct TransactionManager *observer)
{
    t->manager = observer;
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

MESSAGE *TransactionGetLatestResponse(struct Transaction *t)
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
        OnTransactionEvent(t);
    }
    return 0;
}

int TransactionSendMessage(MESSAGE *message)
{
    char s[MAX_MESSAGE_LENGTH] = {0};
    Message2String(s, message);

    return SendOutMessage(s, MessageGetDestAddr(message), MessageGetDestPort(message) );
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

BOOL IfResponseMatchedTransaction(struct Transaction *t, MESSAGE *response)
{
    assert (t != NULL);
    assert (response != NULL);

    MESSAGE *request = TransactionGetRequest(t);
    return MessageViaHeaderBranchMatched(request, response) 
        && MessageCSeqHeaderMethodMatched(request, response);
}

BOOL RequestMethodMatched(struct Transaction *t, MESSAGE *request)
{
    struct RequestLine *origin = MessageGetRequestLine(t->request);
    struct RequestLine *new = MessageGetRequestLine(request);

    return RequestLineMethodMatched(origin, new);
}

BOOL IfRequestMatchTransaction(struct Transaction *t, MESSAGE *request)
{
    assert(t != NULL);
    assert(request != NULL);

    MESSAGE *origin = TransactionGetRequest(t);
    return MessageViaHeaderBranchMatched(origin, request)
        && MessageViaHeaderSendbyMatched(origin, request)
        && RequestMethodMatched(t, request);
}

MESSAGE *TransactionGetRequest(struct Transaction *t)
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

struct TransactionUser *TransactionGetUser(struct Transaction *t)
{
    return t->user;
}

struct Transaction *CallocTransaction(MESSAGE *request, struct TransactionUser *user)
{
    struct Transaction *t = calloc(1, sizeof (struct Transaction));

    t->id = CreateTransactionId();
    t->request = request;
    t->user = user;

    TransactionIdExtract(t->id, request);
    MessageSetOwner(request, t);
    return t;
}

void ResponseWith301(struct Transaction *t)
{
    MESSAGE *moved = Build301Message(t->request);

    TransactionAddResponse(t, moved);
    if (TransactionSendMessage(moved) < 0) {
        RunFsm(t, TRANSACTION_EVENT_TRANSPORT_ERROR);
        return;
    }

    RunFsm(t, TRANSACTION_SEND_301MOVED);
}

void ResponseWith200OK(struct Transaction *t)
{
    MESSAGE *ok = Build200OkMessage((struct Dialog *)t->user, t->request);
    TransactionAddResponse(t, ok);
    TransactionSendMessage(ok);

    RunFsm(t, TRANSACTION_SEND_200OK);
}

void ResponseWith180Ringing(struct Transaction *t)
{
    MESSAGE *ringing = BuildRingingMessage(t->request);

    TransactionAddResponse(t, ringing);
    if (TransactionSendMessage(ringing) < 0) {
        RunFsm(t, TRANSACTION_EVENT_TRANSPORT_ERROR);
        return;
    }

    if (TransactionGetType(t) == TRANSACTION_TYPE_SERVER_NON_INVITE)
        RunFsm(t, TRANSACTION_SEND_1XX);
}

int SendAckRequest(struct Transaction *t)
{
    MESSAGE *ack=BuildAckMessageWithinClientTransaction(t->request);

    TransactionAddResponse(t, ack);
    if (TransactionSendMessage(ack) < 0) {
        RunFsm(t, TRANSACTION_EVENT_TRANSPORT_ERROR);
        return -1;
    }
    return 0;
}

void ReceiveAckRequest(struct Transaction *t)
{
    RunFsm(t, TRANSACTION_EVENT_ACK);
}

void Receive3xxResponse(struct Transaction *t)
{
    RunFsm(t, TRANSACTION_EVENT_3XX);
}

void ReceiveDupRequest(struct Transaction *t, MESSAGE *message)
{
    ResendLatestResponse(t);
}

struct TransactionInitInfo {
    enum TransactionType type;
    enum TransactionState state;
    struct Fsm *fsm;
};

struct TransactionInitInfo TransactionInitInfos[] = {
    {TRANSACTION_TYPE_CLIENT_NON_INVITE, TRANSACTION_STATE_TRYING, &ClientNonInviteTransactionFsm},
    {TRANSACTION_TYPE_CLIENT_INVITE, TRANSACTION_STATE_CALLING, &ClientInviteTransactionFsm},
    {TRANSACTION_TYPE_SERVER_INVITE, TRANSACTION_STATE_PROCEEDING, &ServerInviteTransactionFsm},
    {TRANSACTION_TYPE_SERVER_NON_INVITE, TRANSACTION_STATE_TRYING, &ServerNonInviteTransactionFsm},    
};

struct TransactionInitInfo *GetTransactionInitInfo(enum TransactionType type)
{
    int i = 0;

    assert( 0 <= type && type < TRANSACTION_TYPE_MAX);
    
    for (;i < TRANSACTION_TYPE_MAX;i++) {
        if (type == TransactionInitInfos[i].type)
            return &TransactionInitInfos[i];
    }

    return NULL;
}

struct Transaction *CreateTransaction(MESSAGE *request, struct TransactionUser *user, enum TransactionType type)
{
    struct Transaction *t = CallocTransaction(request, user);
    struct TransactionInitInfo *initInfo = GetTransactionInitInfo(type);

    t->type = type;
    t->state = initInfo->state;
    t->fsm = initInfo->fsm;
    
    if (type == TRANSACTION_TYPE_CLIENT_NON_INVITE || type == TRANSACTION_TYPE_CLIENT_INVITE) {
        if (SendRequestMessage(t) < 0) {
            DestroyTransaction(&t);
            return NULL;
        }
        ClientNonInviteAddRetransmitTimer(t);
        AddTimeoutTimer(t);
    } else if (type == TRANSACTION_TYPE_SERVER_INVITE) {
        MESSAGE *trying = BuildTryingMessage((struct Dialog *)user, t->request);
        TransactionAddResponse(t, trying);
        
        if (TransactionSendMessage(trying) < 0) {
            RunFsm(t, TRANSACTION_EVENT_TRANSPORT_ERROR);
            DestroyTransaction(&t);
            return NULL;
        }
    }
    
    return t;
}

void DestroyResponseMessage(struct Transaction *t)
{
    int length = get_list_len(t->responses);
    int i = 0;

    for (; i < length; i ++) {
        MESSAGE *message = get_data_at(t->responses, i);
        DestroyMessage(&message);
    }

    destroy_list(&t->responses, NULL);
}

void DestroyTransaction(struct Transaction **t)
{
    if ( *t != NULL) {
        DestroyMessage(&(*t)->request); 
        DestroyResponseMessage(*t);
        DestroyTransactionId(&(*t)->id);
        free(*t);
        *t = NULL;
    }
}

struct FsmState ClientNonInviteTryingState = {
    TRANSACTION_STATE_TRYING,
    {
        {TRANSACTION_EVENT_200OK, TRANSACTION_STATE_COMPLETED, {AddWaitForResponseTimer, NotifyUser}},
        {TRANSACTION_EVENT_100TRYING,TRANSACTION_STATE_PROCEEDING, {ResetRetransmitTimer}},
        {TRANSACTION_EVENT_401UNAUTHORIZED, TRANSACTION_STATE_COMPLETED, {AddWaitForResponseTimer, NotifyUser}},
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
        {TRANSACTION_EVENT_200OK, TRANSACTION_STATE_COMPLETED,{AddWaitForResponseTimer}},
        {TRANSACTION_EVENT_100TRYING,TRANSACTION_STATE_PROCEEDING},
        {TRANSACTION_EVENT_180RINGING, TRANSACTION_STATE_PROCEEDING},
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
        {TRANSACTION_EVENT_200OK, TRANSACTION_STATE_TERMINATED,{NotifyUser,}},
        {TRANSACTION_EVENT_100TRYING,TRANSACTION_STATE_PROCEEDING,{ResetRetransmitTimer}},
        {TRANSACTION_EVENT_180RINGING, TRANSACTION_STATE_PROCEEDING,{NotifyUser}},
        {TRANSACTION_EVENT_RETRANSMIT_TIMER_FIRED,TRANSACTION_STATE_CALLING,{
                SendRequestMessage,
                IncRetransmit,
                ClientInviteAddRetransmitTimer}},
        {TRANSACTION_EVENT_TIMEOUT_TIMER_FIRED,TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_EVENT_TRANSPORT_ERROR, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_EVENT_3XX, TRANSACTION_STATE_COMPLETED, {AddWaitForResponseTimer}},
        {TRANSACTION_EVENT_MAX}
    }
};

struct FsmState ClientInviteProceedingState = {
    TRANSACTION_STATE_PROCEEDING,
    {
        {TRANSACTION_EVENT_3XX, TRANSACTION_STATE_COMPLETED, {AddWaitForResponseTimer}},
        {TRANSACTION_EVENT_200OK, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_EVENT_100TRYING, TRANSACTION_STATE_PROCEEDING},
        {TRANSACTION_EVENT_180RINGING, TRANSACTION_STATE_PROCEEDING,{NotifyUser}},
        {TRANSACTION_EVENT_MAX},
    }
};

struct FsmState ClientInviteCompletedState = {
    TRANSACTION_STATE_COMPLETED,
    {
        {TRANSACTION_EVENT_3XX, TRANSACTION_STATE_COMPLETED, {SendAckRequest}},
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
        {TRANSACTION_EVENT_INVITE, TRANSACTION_STATE_PROCEEDING,{ResendLatestResponse}},
        {TRANSACTION_EVENT_TRANSPORT_ERROR, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_SEND_200OK, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_SEND_301MOVED, TRANSACTION_STATE_COMPLETED, {ClientNonInviteAddRetransmitTimer, AddTimeoutTimer}},        
        {TRANSACTION_EVENT_MAX}
    }
};

struct FsmState ServerInviteCompletedState = {
    TRANSACTION_STATE_COMPLETED,
    {
        {TRANSACTION_EVENT_INVITE, TRANSACTION_STATE_COMPLETED, {ResendLatestResponse}},
        {TRANSACTION_EVENT_RETRANSMIT_TIMER_FIRED, TRANSACTION_STATE_COMPLETED, {ResendLatestResponse}},
        {TRANSACTION_EVENT_TIMEOUT_TIMER_FIRED, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_EVENT_TRANSPORT_ERROR, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_EVENT_ACK, TRANSACTION_STATE_CONFIRMED, {AddWaitForResponseTimer}},
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
        {TRANSACTION_SEND_1XX, TRANSACTION_STATE_PROCEEDING},
        {TRANSACTION_SEND_200OK, TRANSACTION_STATE_COMPLETED, {
                AddServerNonInviteWaitRequestRetransmitTimer}},
        {TRANSACTION_EVENT_MAX}
    }
};

struct FsmState ServerNonInviteProceedingState = {
    TRANSACTION_STATE_PROCEEDING,
    {
        {TRANSACTION_SEND_1XX, TRANSACTION_STATE_PROCEEDING},
        {TRANSACTION_EVENT_TRANSPORT_ERROR, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_SEND_200OK, TRANSACTION_STATE_COMPLETED, {
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

void RemoveTerminatedTransaction(struct Transaction *t)
{
    if (TransactionGetState(t) == TRANSACTION_STATE_TERMINATED)
        if (t != NULL && t->manager != NULL) {
            RemoveTransaction(t);
        }
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
        RemoveTerminatedTransaction(t);
        return;
    } 

    printf("Transaction Event Handle Error: %d, %d\n", TransactionGetState(t), event);
    assert (0);
}

void TransactionDump(struct Transaction *t)
{
    printf("%s:%s:%d\n", TransactionIdGetBranch(t->id), TransactionIdGetMethod(t->id), t->state);
}
