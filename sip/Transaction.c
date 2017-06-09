#include <stdio.h>
#include <assert.h>

#include "Bool.h"
#include "Log.h"
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
#include "utils/Maps.h"

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
    int events;
    enum TransactionState nextState;
    TransactionEventAction actions[TRANSACTION_ACTIONS_MAX + 1];
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

void AddResponse(struct Transaction *t, MESSAGE *message)
{
    put_in_list(&t->responses, message);
}

void SetTransactionObserver(struct Transaction *t, struct TransactionManager *observer)
{
    t->manager = observer;
}

enum TransactionState GetTransactionState(struct Transaction *t)
{
    assert(t != NULL);
    return t->state;
}

void SetTransactionState(struct Transaction *t, enum TransactionState state)
{
    assert(t != NULL);
    t->state = state;
}

struct TransactionId *GetTransactionId(struct Transaction *t)
{
    assert(t != NULL);
    return t->id;
}

MESSAGE *GetLatestResponseImpl(struct Transaction *t)
{
    int length = get_list_len(t->responses);
    return get_data_at(t->responses, length - 1);
}
MESSAGE *(*GetLatestResponse)(struct Transaction *t) = GetLatestResponseImpl;

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

int ResetRetransmits(struct Transaction *t)
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

int InformTU(struct Transaction *t)
{
    if (t && t->user) {
        OnTransactionEvent((struct Dialog *)t->user, t->curEvent, GetLatestResponse(t));
    }
    return 0;
}

int SendTransactionMessage(struct Transaction *t, struct Message *message)
{
    if (SendMessage(message) < 0) {
         RunFsm(t, TRANSACTION_EVENT_TRANSPORT_ERROR);
         return -1;
    }

    return 0;
}

int SendRequestMessage(struct Transaction *t)
{
    assert(t != NULL);
    assert(t->request != NULL);

    return SendTransactionMessage(t, t->request);
}

int ResendLatestResponse(struct Transaction *t)
{
    return SendTransactionMessage(t, GetLatestResponse(t));
}

BOOL ResponseTransactionMatched(struct Transaction *t, MESSAGE *response)
{
    assert (t != NULL);
    assert (response != NULL);

    MESSAGE *request = GetTransactionRequest(t);
    return MessageViaHeaderBranchMatched(request, response) 
        && MessageCSeqHeaderMethodMatched(request, response);
}

BOOL RequestMethodMatched(struct Transaction *t, MESSAGE *request)
{
    struct RequestLine *origin = MessageGetRequestLine(t->request);
    struct RequestLine *new = MessageGetRequestLine(request);

    return RequestLineMethodMatched(origin, new);
}

BOOL RequestTransactionMatched(struct Transaction *t, MESSAGE *request)
{
    assert(t != NULL);
    assert(request != NULL);

    MESSAGE *origin = GetTransactionRequest(t);
    return MessageViaHeaderBranchMatched(origin, request)
        && MessageViaHeaderSendbyMatched(origin, request)
        && RequestMethodMatched(t, request);
}

MESSAGE *GetTransactionRequestImpl(struct Transaction *t)
{
    assert (t != NULL);
    return t->request;
}
struct Message *(*GetTransactionRequest)(struct Transaction *t) = GetTransactionRequestImpl;

enum TransactionType GetTransactionType(struct Transaction *t)
{
    return t->type;
}

int GetCurrentEvent(struct Transaction *t)
{
    return t->curEvent;
}

struct TransactionUser *GetTransactionUser(struct Transaction *t)
{
    return t->user;
}

int ResponseWith(struct Transaction *t, struct Message *message, int event)
{
    AddResponse(t, message);

    if (SendTransactionMessage(t, message) < 0) {
        return -1;
    }

    RunFsm(t, event);

    return 0;
}

void Response(struct Transaction *t, int e)
{
    struct Message *message = NULL;

    if (e == TRANSACTION_SEND_OK) {
        message = BuildResponse(t->request, STATUS_CODE_OK);
    } else if (e == TRANSACTION_SEND_MOVED) {
        message = BuildResponse(t->request,STATUS_CODE_MOVED_PERMANENTLY);
    } else if (e == TRANSACTION_SEND_RINGING) {
        message = BuildResponse(t->request, STATUS_CODE_RINGING);
    }
        
    ResponseWith(t, message, e);
}

int ResponseWith100Trying(struct Transaction *t)
{
    MESSAGE *trying = BuildResponse(t->request, STATUS_CODE_TRYING);
    ResponseWith(t, trying, TRANSACTION_SEND_TRYING);
    return 0;
}

int SendAckRequest(struct Transaction *t)
{

    char *ipaddr = GetMessageAddr(t->request);
    int port = GetMessagePort(t->request);
    MESSAGE *last_response = GetLatestResponse(t);
    char *tag = NULL;
    
    if (last_response != NULL) {
        tag = MessageGetToTag(last_response);
    }
    
    MESSAGE *ack=BuildAckMessageWithTag(t->request, ipaddr, port, tag);

    AddResponse(t, ack);
    return SendTransactionMessage(t, ack);
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
    struct Fsm *fsm;
};

struct TransactionInitInfo TransactionInitInfos[] = {
    {TRANSACTION_TYPE_CLIENT_NON_INVITE, &ClientNonInviteTransactionFsm},
    {TRANSACTION_TYPE_CLIENT_INVITE,     &ClientInviteTransactionFsm},
    {TRANSACTION_TYPE_SERVER_INVITE,     &ServerInviteTransactionFsm},
    {TRANSACTION_TYPE_SERVER_NON_INVITE, &ServerNonInviteTransactionFsm},    
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
    struct Transaction *t = calloc(1, sizeof (struct Transaction));
    struct TransactionInitInfo *initInfo = GetTransactionInitInfo(type);

    t->request = request;
    t->user = user;
    t->type = type;
    t->state = TRANSACTION_STATE_INIT;
    t->fsm = initInfo->fsm;
    
    t->id = CreateTransactionId();
    ExtractTransactionId(t->id, request);

    MessageSetOwner(request, t);
    
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

void RemoveTerminatedTransaction(struct Transaction *t)
{
    if (GetTransactionState(t) == TRANSACTION_STATE_TERMINATED)
        if (t != NULL && t->manager != NULL) {
            RemoveTransaction(t);
        }
}

void DumpTransaction(struct Transaction *t)
{
    printf("Branch: %s| Method: %s | State: %s\n",
           TransactionIdGetBranch(t->id),
           TransactionIdGetMethod(t->id),
           TransactionState2String(t->state));
}

struct FsmState ClientNonInviteInitState = {
    TRANSACTION_STATE_INIT,
    {
        {TRANSACTION_EVENT_INIT, TRANSACTION_STATE_TRYING,{
                SendRequestMessage,
                ClientNonInviteAddRetransmitTimer,
                AddTimeoutTimer}},
        {TRANSACTION_EVENT_TRANSPORT_ERROR, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_EVENT_MAX}
    }
};

struct FsmState ClientNonInviteTryingState = {
    TRANSACTION_STATE_TRYING,
    {
        {TRANSACTION_EVENT_2XX, TRANSACTION_STATE_COMPLETED, {AddWaitForResponseTimer, InformTU}},
        {TRANSACTION_EVENT_1XX,TRANSACTION_STATE_PROCEEDING, {ResetRetransmits}},
        {TRANSACTION_EVENT_4XX, TRANSACTION_STATE_COMPLETED, {AddWaitForResponseTimer, InformTU}},
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
        {TRANSACTION_EVENT_2XX, TRANSACTION_STATE_COMPLETED,{AddWaitForResponseTimer}},
        {TRANSACTION_EVENT_1XX,TRANSACTION_STATE_PROCEEDING},
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
        &ClientNonInviteInitState,
        &ClientNonInviteTryingState,
        &ClientNonInviteProceedingState,
        &ClientNonInviteCompletedState,
        NULL,
    }
};

struct FsmState ClientInviteInitState = {
    TRANSACTION_STATE_INIT,
    {
        {TRANSACTION_EVENT_INIT, TRANSACTION_STATE_CALLING,{
                SendRequestMessage,
                ClientNonInviteAddRetransmitTimer,
                AddTimeoutTimer}},
        {TRANSACTION_EVENT_TRANSPORT_ERROR, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_EVENT_MAX}
    }
};

struct FsmState ClientInviteCallingState = {
    TRANSACTION_STATE_CALLING,
    {
        {
            TRANSACTION_EVENT_2XX,
            TRANSACTION_STATE_TERMINATED,
            {InformTU}
        },

        {
            TRANSACTION_EVENT_1XX,
            TRANSACTION_STATE_PROCEEDING,
            {ResetRetransmits}
        },

        {
            TRANSACTION_EVENT_RETRANSMIT_TIMER_FIRED,
            TRANSACTION_STATE_CALLING,
            {
                SendRequestMessage,
                IncRetransmit,
                ClientInviteAddRetransmitTimer
            }
        },
        
        {
            TRANSACTION_EVENT_TIMEOUT_TIMER_FIRED,
            TRANSACTION_STATE_TERMINATED
        },

        {
            TRANSACTION_EVENT_TRANSPORT_ERROR,
            TRANSACTION_STATE_TERMINATED
        },
        
        {
            TRANSACTION_EVENT_3XX | TRANSACTION_EVENT_5XX | TRANSACTION_EVENT_6XX,
            TRANSACTION_STATE_COMPLETED,
            {
                AddWaitForResponseTimer
            }
        },
        
        {TRANSACTION_EVENT_MAX}
    }
};

struct FsmState ClientInviteProceedingState = {
    TRANSACTION_STATE_PROCEEDING,
    {
        {
            TRANSACTION_EVENT_3XX | TRANSACTION_EVENT_4XX | TRANSACTION_EVENT_5XX | TRANSACTION_EVENT_6XX,
            TRANSACTION_STATE_COMPLETED,
            {
                AddWaitForResponseTimer,
                SendAckRequest
            }
        },
        
        {TRANSACTION_EVENT_2XX, TRANSACTION_STATE_TERMINATED,{InformTU}},
        {TRANSACTION_EVENT_1XX, TRANSACTION_STATE_PROCEEDING, {InformTU}},
        {TRANSACTION_EVENT_MAX},
    }
};

struct FsmState ClientInviteCompletedState = {
    TRANSACTION_STATE_COMPLETED,
    {
        {
            TRANSACTION_EVENT_3XX | TRANSACTION_EVENT_4XX | TRANSACTION_EVENT_5XX | TRANSACTION_EVENT_6XX,
            TRANSACTION_STATE_COMPLETED,
            {
                SendAckRequest,
                AddWaitForResponseTimer,
            }
        },
        {TRANSACTION_EVENT_TRANSPORT_ERROR, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_EVENT_WAIT_FOR_RESPONSE_TIMER_FIRED, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_EVENT_MAX},
    }
};

struct Fsm ClientInviteTransactionFsm = {
    {
        &ClientInviteInitState,
        &ClientInviteCallingState,
        &ClientInviteProceedingState,
        &ClientInviteCompletedState,
        NULL,
    }
};

struct FsmState ServerInviteInitState = {
    TRANSACTION_STATE_INIT,
    {
        {TRANSACTION_EVENT_INIT, TRANSACTION_STATE_PROCEEDING, {ResponseWith100Trying}},
        {TRANSACTION_EVENT_TRANSPORT_ERROR, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_EVENT_MAX}
    }
};

struct FsmState ServerInviteProceedingState = {
    TRANSACTION_STATE_PROCEEDING,
    {
        {TRANSACTION_SEND_TRYING, TRANSACTION_STATE_PROCEEDING},
        {TRANSACTION_SEND_RINGING, TRANSACTION_STATE_PROCEEDING},
        {TRANSACTION_EVENT_INVITE, TRANSACTION_STATE_PROCEEDING,{ResendLatestResponse}},
        {TRANSACTION_EVENT_TRANSPORT_ERROR, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_SEND_REQUEST_TERMINATED, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_SEND_OK, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_SEND_MOVED, TRANSACTION_STATE_COMPLETED, {ClientNonInviteAddRetransmitTimer, AddTimeoutTimer}},
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
        &ServerInviteInitState,
        &ServerInviteProceedingState,
        &ServerInviteCompletedState,
        &ServerInviteConfirmedState,
        NULL,
    }
};

struct FsmState ServerNonInviteInitState = {
    TRANSACTION_STATE_INIT,
    {
        {TRANSACTION_EVENT_INIT, TRANSACTION_STATE_TRYING},
        {TRANSACTION_EVENT_MAX}
    }
};

struct FsmState ServerNonInviteTryingState = {
    TRANSACTION_STATE_TRYING,
    {
        {TRANSACTION_SEND_RINGING, TRANSACTION_STATE_PROCEEDING},
        {TRANSACTION_SEND_OK, TRANSACTION_STATE_COMPLETED, {
                AddServerNonInviteWaitRequestRetransmitTimer}},
        {TRANSACTION_EVENT_MAX}
    }
};

struct FsmState ServerNonInviteProceedingState = {
    TRANSACTION_STATE_PROCEEDING,
    {
        {TRANSACTION_SEND_RINGING, TRANSACTION_STATE_PROCEEDING},
        {TRANSACTION_EVENT_TRANSPORT_ERROR, TRANSACTION_STATE_TERMINATED},
        {TRANSACTION_SEND_OK, TRANSACTION_STATE_COMPLETED, {
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
        &ServerNonInviteInitState,
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

void TransactionHandleEvent(struct Transaction *t, int event, struct FsmStateEventEntry *entry)
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

struct FsmStateEventEntry *LocateEventEntry(struct Transaction *t, int event)
{
    int i = 0;
    struct FsmStateEventEntry *entrys = NULL;
    struct FsmState *fsmState = NULL;

    if ((fsmState = LocateFsmState(t)) == NULL) return NULL;

    entrys = fsmState->entrys;
    for ( i = 0; entrys[i].events != TRANSACTION_EVENT_MAX; i++) {
        if ((entrys[i].events & event) != 0) {
            return &entrys[i];
        }
    }

    return NULL;
}

struct IntStringMap TransactionStateStringMap[] = {
    INT_STRING_MAP(TRANSACTION_STATE_INIT),
    INT_STRING_MAP(TRANSACTION_STATE_TRYING),
    INT_STRING_MAP(TRANSACTION_STATE_CALLING),
    INT_STRING_MAP(TRANSACTION_STATE_PROCEEDING),
    INT_STRING_MAP(TRANSACTION_STATE_COMPLETED),
    INT_STRING_MAP(TRANSACTION_STATE_CONFIRMED),
    INT_STRING_MAP(TRANSACTION_STATE_TERMINATED),
    {-1,""}
};

struct IntStringMap TransactionEventStringMap[] = {
    INT_STRING_MAP(TRANSACTION_EVENT_NEW),
    INT_STRING_MAP(TRANSACTION_EVENT_INIT),
    INT_STRING_MAP(TRANSACTION_EVENT_ACK),
    INT_STRING_MAP(TRANSACTION_EVENT_1XX),
    INT_STRING_MAP(TRANSACTION_EVENT_2XX),
    INT_STRING_MAP(TRANSACTION_EVENT_3XX),
    INT_STRING_MAP(TRANSACTION_EVENT_4XX),
    INT_STRING_MAP(TRANSACTION_EVENT_5XX),
    INT_STRING_MAP(TRANSACTION_EVENT_6XX),
    INT_STRING_MAP(TRANSACTION_EVENT_INVITE),
    INT_STRING_MAP(TRANSACTION_EVENT_BYE),
    
    INT_STRING_MAP(TRANSACTION_SEND_RINGING),
    INT_STRING_MAP(TRANSACTION_SEND_TRYING),
    INT_STRING_MAP(TRANSACTION_SEND_OK),
    INT_STRING_MAP(TRANSACTION_SEND_MOVED),
    INT_STRING_MAP(TRANSACTION_SEND_REQUEST_TERMINATED),

    INT_STRING_MAP(TRANSACTION_EVENT_RETRANSMIT_TIMER_FIRED),
    INT_STRING_MAP(TRANSACTION_EVENT_TIMEOUT_TIMER_FIRED),
    INT_STRING_MAP(TRANSACTION_EVENT_WAIT_FOR_RESPONSE_TIMER_FIRED),
    INT_STRING_MAP(TRANSACTION_EVENT_WAIT_REQUEST_RETRANSMITS_TIMER_FIRED),
    INT_STRING_MAP(TRANSACTION_EVENT_TRANSPORT_ERROR),
    INT_STRING_MAP(TRANSACTION_EVENT_MAX),
    {-1, ""}
};

char *TransactionState2String(enum TransactionState s)
{
    return IntMap2String(s, TransactionStateStringMap);
}

char *TransactionEvent2String(int e)
{
    return IntMap2String(e, TransactionEventStringMap);
}

void RunFsm(struct Transaction *t, int event)
{
    assert(t != NULL);
    struct FsmStateEventEntry *entry = NULL;
    
    LOG("Transaction Event: State %s, event %s\n",
           TransactionState2String(GetTransactionState(t)),
           TransactionEvent2String(event));

    if ((entry = LocateEventEntry(t, event)) != NULL) {
        TransactionHandleEvent(t, event, entry);            
        RemoveTerminatedTransaction(t);
        return;
    } 

    printf("Transaction Event: State %s, event %s\n",
           TransactionState2String(GetTransactionState(t)),
           TransactionEvent2String(event));

    assert (0);
}

int MapStatusCodeToEvent(int statusCode)
{    
    if (100 <= statusCode && statusCode < 200) {
        return TRANSACTION_EVENT_1XX;
    } else if ( 200 <= statusCode && statusCode < 300) {
        return TRANSACTION_EVENT_2XX;
    } else if ( 300 <= statusCode && statusCode < 400) {
        return TRANSACTION_EVENT_3XX;
    } else if ( 400 <= statusCode && statusCode < 500) {
        return TRANSACTION_EVENT_4XX;
    } else if ( 500 <= statusCode && statusCode < 600) {
        return TRANSACTION_EVENT_5XX;
    } else if ( 600 <= statusCode && statusCode < 700) {
        return TRANSACTION_EVENT_6XX;
    }
    
    return statusCode;
}

void RunFsmByStatusCode(struct Transaction *t, int statusCode)
{
    RunFsm(t, MapStatusCodeToEvent(statusCode)); 
}
