#include "Timer.h"
#include "Bool.h"

struct Message;
struct Transaction;
struct TransactionUser;
struct TransactionManager;

typedef int (*TransactionAction)(struct Transaction *t);

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
};

enum TransactionEvent {
    TRANSACTION_EVENT_200OK,
    TRANSACTION_EVENT_ACK,
    TRANSACTION_EVENT_100TRYING,
    TRANSACTION_EVENT_180RINGING,
    TRANSACTION_EVENT_401UNAUTHORIZED,
    TRANSACTION_EVENT_3XX,
    TRANSACTION_EVENT_INVITE,
    
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

struct Transaction *CreateClientNonInviteTransaction(struct Message *request, struct TransactionUser *user);
struct Transaction *CreateClientInviteTransaction(struct Message *request, struct TransactionUser *user);
struct Transaction *CreateServerInviteTransaction(struct Message *request, struct TransactionUser *user);
struct Transaction *CreateServerNonInviteTransaction(struct Message *request, struct TransactionUser *user);
void DestroyTransaction(struct Transaction **t);

int TransactionSendMessage(struct Message *message);
void ResponseWith180Ringing(struct Transaction *t);
void ResponseWith200OK(struct Transaction *t);
void ResponseWith301(struct Transaction *t);

void ReceiveAckRequest(struct Transaction *t);
void Receive3xxResponse(struct Transaction *t);
void ReceiveDupRequest(struct Transaction *t, struct Message *message);

struct TransactionId *TransactionGetId(struct Transaction *t);
struct Message * TransactionGetRequest(struct Transaction *t);
struct Message *TransactionGetLatestResponse(struct Transaction *t);
void TransactionAddResponse(struct Transaction *t, struct Message *message);
enum TransactionEvent TransactionGetCurrentEvent(struct Transaction *t);

void RunFsm(struct Transaction *t, enum TransactionEvent event);
void TransactionSetObserver(struct Transaction *t, struct TransactionManager *manager);
enum TransactionState TransactionGetState(struct Transaction *t);
struct TransactionUser *TransactionGetUser(struct Transaction *t);
enum TransactionType TransactionGetType(struct Transaction *t);

BOOL IfResponseMatchedTransaction(struct Transaction *t, struct Message *response);
BOOL IfRequestMatchTransaction(struct Transaction *t, struct Message *m);

void TransactionDump(struct Transaction *t);
