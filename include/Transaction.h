#include "Timer.h"

struct Message;
struct Transaction;
struct TransactionManagerNotifiers;
struct TransactionUserNotifiers;

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
    TRANSACTION_EVENT_200OK_RECEIVED,
    TRANSACTION_EVENT_100TRYING_RECEIVED,
    TRANSACTION_EVENT_180RINGING_RECEIVED,

    TRANSACTION_EVENT_200OK_SENT,
    TRANSACTION_EVENT_INVITE_RECEIVED,
    TRANSACTION_EVENT_RETRANSMIT_TIMER_FIRED,
    TRANSACTION_EVENT_TIMEOUT_TIMER_FIRED,
    TRANSACTION_EVENT_WAIT_FOR_RESPONSE_TIMER_FIRED,
    TRANSACTION_EVENT_TRANSPORT_ERROR,
    TRANSACTION_EVENT_MAX,
};

struct Transaction *CreateClientTransaction(struct Message *request, struct TransactionUserNotifiers *user);
struct Transaction *CreateServerTransaction(struct Message *request, struct TransactionUserNotifiers *user);
void DestoryTransaction(struct Transaction **t);

int TransactionSendMessage(struct Message *message);
void ResponseWith180Ringing(struct Transaction *t);
void ResponseWith200OK(struct Transaction *t);

enum TransactionState TransactionGetState(struct Transaction *t);
struct Message * TransactionGetRequest(struct Transaction *t);
void TransactionAddResponse(struct Transaction *t, struct Message *message);
void RunFsm(struct Transaction *t, enum TransactionEvent event);
void TransactionSetNotifiers(struct Transaction *t, struct TransactionManagerNotifiers *notifiers);
enum TransactionEvent TransactionGetCurrentEvent(struct Transaction *t);
struct TransactionUserNotifiers *TransactionGetUser(struct Transaction *t);
struct Message *TransactionGetLatestResponse(struct Transaction *t);
enum TransactionType TransactionGetType(struct Transaction *t);
