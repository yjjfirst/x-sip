#include "Timer.h"

struct Message;
struct Transaction;
struct TransactionManagerInterface;
struct TransactionOwnerInterface;

typedef int (*TransactionAction)(struct Transaction *t);

enum TransactionState {
    TRANSACTION_STATE_TRYING, 
    TRANSACTION_STATE_CALLING,
    TRANSACTION_STATE_PROCEEDING,
    TRANSACTION_STATE_COMPLETED,
    TRANSACTION_STATE_TERMINATED,
    TRANSACTION_STATE_MAX,
};

enum TransactionType {
    TRANSACTION_TYPE_CLIENT_INVITE,
    TRANSACTION_TYPE_CLIENT_NON_INVITE,
};

enum TransactionEvent {
    TRANSACTION_EVENT_200OK,
    TRANSACTION_EVENT_100TRYING,
    TRANSACTION_EVENT_180RINGING,
    TRANSACTION_EVENT_RETRANSMIT_TIMER_FIRED,
    TRANSACTION_EVENT_TIMEOUT_TIMER_FIRED,
    TRANSACTION_EVENT_WAIT_FOR_RESPONSE_TIMER_FIRED,
    TRANSACTION_EVENT_TRANSPORT_ERROR,
    TRANSACTION_EVENT_MAX,
};

struct Transaction *CreateTransaction(struct Message *request, struct TransactionOwnerInterface *owner);
void DestoryTransaction(struct Transaction **t);
void TransactionSetTimerManager(TimerAddFunc adder);
void TransactionRemoveTimer();

enum TransactionState TransactionGetState(struct Transaction *t);
struct Message * TransactionGetRequest(struct Transaction *t);
void TransactionAddResponse(struct Transaction *t, struct Message *message);
void RunFSM(struct Transaction *t, enum TransactionEvent event);
void TransactionSetManagerInterface(struct Transaction *t, struct TransactionManagerInterface *manager);
enum TransactionEvent TransactionGetCurrentEvent(struct Transaction *t);
struct TransactionOwnerInterface *TransactionGetOwner(struct Transaction *t);
struct Message *TransactionGetLatestResponse(struct Transaction *t);
enum TransactionType TransactionGetType(struct Transaction *t);
