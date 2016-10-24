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
    TRANSACTION_EVENT_200OK,
    TRANSACTION_EVENT_ACK,
    TRANSACTION_EVENT_100TRYING,
    TRANSACTION_EVENT_180RINGING,
    TRANSACTION_EVENT_401UNAUTHORIZED,
    TRANSACTION_EVENT_3XX,
    TRANSACTION_EVENT_INVITE,
    TRANSACTION_EVENT_BYE,
    
    TRANSACTION_SEND_1XX,
    TRANSACTION_SEND_200OK,
    TRANSACTION_SEND_301MOVED,

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

void ResponseWith180Ringing(struct Transaction *t);
void ResponseWith200OK(struct Transaction *t);
void ResponseWith301(struct Transaction *t);

void ReceiveAckRequest(struct Transaction *t);
void Receive3xxResponse(struct Transaction *t);
void ReceiveDupRequest(struct Transaction *t, struct Message *message);

struct TransactionId *GetTransactionId(struct Transaction *t);
struct Message * GetTransactionRequest(struct Transaction *t);
struct Message *GetLatestResponse(struct Transaction *t);
void AddResponse(struct Transaction *t, struct Message *message);
enum TransactionEvent GetCurrentEvent(struct Transaction *t);

void RunFsm(struct Transaction *t, enum TransactionEvent event);
void SetTransactionObserver(struct Transaction *t, struct TransactionManager *manager);
enum TransactionState GetTransactionState(struct Transaction *t);
struct TransactionUser *GetTransactionUser(struct Transaction *t);
enum TransactionType GetTransactionType(struct Transaction *t);

BOOL ResponseTransactionMatched(struct Transaction *t, struct Message *response);
BOOL RequestTransactionMatched(struct Transaction *t, struct Message *m);

void DumpTransaction(struct Transaction *t);
