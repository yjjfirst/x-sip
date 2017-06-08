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

#define TRANSACTION_SEND_RINGING 1<<0
#define TRANSACTION_SEND_TRYING  1<<1
#define TRANSACTION_SEND_OK 1<<2
#define TRANSACTION_SEND_MOVED 1<<3
#define TRANSACTION_SEND_REQUEST_TERMINATED 1<<4

#define TRANSACTION_EVENT_NEW 1<<5
#define TRANSACTION_EVENT_INIT 1<<6
#define TRANSACTION_EVENT_ACK 1<<7
#define TRANSACTION_EVENT_INVITE 1<<8
#define TRANSACTION_EVENT_CANCEL 1<<9
#define TRANSACTION_EVENT_BYE 1<<10
#define TRANSACTION_EVENT_1XX 1<<11
#define TRANSACTION_EVENT_2XX 1<<12
#define TRANSACTION_EVENT_3XX 1<<13
#define TRANSACTION_EVENT_4XX 1<<14
#define TRANSACTION_EVENT_5XX 1<<15
#define TRANSACTION_EVENT_6XX 1<<16  
#define TRANSACTION_EVENT_RETRANSMIT_TIMER_FIRED 1<<17
#define TRANSACTION_EVENT_TIMEOUT_TIMER_FIRED 1<<18
#define TRANSACTION_EVENT_WAIT_FOR_RESPONSE_TIMER_FIRED 1<<19
#define TRANSACTION_EVENT_WAIT_REQUEST_RETRANSMITS_TIMER_FIRED 1<<20
#define TRANSACTION_EVENT_TRANSPORT_ERROR 1<<21

#define TRANSACTION_EVENT_MAX 32

extern void (*OnTransactionEvent)(struct Dialog *dialog, int event, struct Message *messge);
struct Transaction *CreateTransaction(struct Message *request, struct TransactionUser *user, enum TransactionType type);
void DestroyTransaction(struct Transaction **t);

int ResponseWith(struct Transaction *t, struct Message *message, int event);
void Response(struct Transaction *t, int e);

void ReceiveAckRequest(struct Transaction *t);
void Receive3xxResponse(struct Transaction *t);
void ReceiveDupRequest(struct Transaction *t, struct Message *message);

struct TransactionId *GetTransactionId(struct Transaction *t);
extern struct Message *(*GetTransactionRequest)(struct Transaction *t);
extern struct Message *(*GetLatestResponse)(struct Transaction *t);
void AddResponse(struct Transaction *t, struct Message *message);
int GetCurrentEvent(struct Transaction *t);

void RunFsm(struct Transaction *t, int event);
void RunFsmByStatusCode(struct Transaction *t, int statusCode);

void SetTransactionObserver(struct Transaction *t, struct TransactionManager *manager);
enum TransactionState GetTransactionState(struct Transaction *t);
void SetTransactionState(struct Transaction *t, enum TransactionState state);
struct TransactionUser *GetTransactionUser(struct Transaction *t);
enum TransactionType GetTransactionType(struct Transaction *t);

BOOL ResponseTransactionMatched(struct Transaction *t, struct Message *response);
BOOL RequestTransactionMatched(struct Transaction *t, struct Message *m);
int MapStatusCodeToEvent(int statusCode);


void DumpTransaction(struct Transaction *t);
char *TransactionState2String(enum TransactionState s);
char *TransactionEvent2String(int e);
