#include "Timer.h"
#include "Bool.h"

struct Message;
struct Transaction;
struct TransactionUser;
struct TransactionManager;
struct Dialog;

typedef int (*TransactionEventAction)(struct Transaction *t);

struct TransactionUser {
};

enum TransactionState {
    TRANSACTION_STATE_INIT,
    TRANSACTION_STATE_TRYING, 
    TRANSACTION_STATE_CALLING,
    TRANSACTION_STATE_PROCEEDING,
    TRANSACTION_STATE_COMPLETED,
    TRANSACTION_STATE_CONFIRMED,
    TRANSACTION_STATE_TERMINATED,
    TRANSACTION_STATE_MAX,
};

enum TransactionType {
    TRANSACTION_TYPE_CLIENT_INVITE,
    TRANSACTION_TYPE_CLIENT_NON_INVITE,
    TRANSACTION_TYPE_SERVER_INVITE,
    TRANSACTION_TYPE_SERVER_NON_INVITE,
    TRANSACTION_TYPE_MAX,
};

enum TransactionEvent {
    TRANSACTION_EVENT_NEW,
    TRANSACTION_EVENT_INIT,
    TRANSACTION_EVENT_ACK,
    TRANSACTION_EVENT_INVITE,
    TRANSACTION_EVENT_CANCELED,
    TRANSACTION_EVENT_BYE,
    
    TRANSACTION_SEND_RINGING,
    TRANSACTION_SEND_TRYING,
    TRANSACTION_SEND_OK,
    TRANSACTION_SEND_MOVED,
    TRANSACTION_SEND_REQUEST_TERMINATED,
    
    TRANSACTION_EVENT_1XX,
    TRANSACTION_EVENT_2XX,
    TRANSACTION_EVENT_3XX,
    TRANSACTION_EVENT_4XX,
    TRANSACTION_EVENT_5XX,
    TRANSACTION_EVENT_6XX,


    
    TRANSACTION_EVENT_RETRANSMIT_TIMER_FIRED,
    TRANSACTION_EVENT_TIMEOUT_TIMER_FIRED,
    TRANSACTION_EVENT_WAIT_FOR_RESPONSE_TIMER_FIRED,
    TRANSACTION_EVENT_WAIT_REQUEST_RETRANSMITS_TIMER_FIRED,
    TRANSACTION_EVENT_TRANSPORT_ERROR,
    TRANSACTION_EVENT_MAX,
};

extern void (*OnTransactionEvent)(struct Dialog *dialog, int event, struct Message *messge);
struct Transaction *CreateTransaction(struct Message *request, struct TransactionUser *user, enum TransactionType type);
void DestroyTransaction(struct Transaction **t);

int ResponseWith(struct Transaction *t, struct Message *message, enum TransactionEvent event);
void Response(struct Transaction *t, enum TransactionEvent e);

void ReceiveAckRequest(struct Transaction *t);
void Receive3xxResponse(struct Transaction *t);
void ReceiveDupRequest(struct Transaction *t, struct Message *message);

struct TransactionId *GetTransactionId(struct Transaction *t);
extern struct Message *(*GetTransactionRequest)(struct Transaction *t);
extern struct Message *(*GetLatestResponse)(struct Transaction *t);
void AddResponse(struct Transaction *t, struct Message *message);
enum TransactionEvent GetCurrentEvent(struct Transaction *t);

void RunFsm(struct Transaction *t, enum TransactionEvent event);
void RunFsmByStatusCode(struct Transaction *t, int statusCode);

void SetTransactionObserver(struct Transaction *t, struct TransactionManager *manager);
enum TransactionState GetTransactionState(struct Transaction *t);
void SetTransactionState(struct Transaction *t, enum TransactionState state);
struct TransactionUser *GetTransactionUser(struct Transaction *t);
enum TransactionType GetTransactionType(struct Transaction *t);

BOOL ResponseTransactionMatched(struct Transaction *t, struct Message *response);
BOOL RequestTransactionMatched(struct Transaction *t, struct Message *m);
enum TransactionEvent MapStatusCodeToEvent(int statusCode);


void DumpTransaction(struct Transaction *t);
char *TransactionState2String(enum TransactionState s);
char *TransactionEvent2String(enum TransactionEvent e);
