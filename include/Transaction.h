struct Message;
struct Transaction;
struct TransactionNotifyInterface;

typedef void (*TimerCallback)(void *transaction);
typedef void (*TransactionTimerAdder)(void *transaction, int ms, TimerCallback onTime);
typedef int (*TransactionAction)(struct Transaction *t);

enum TransactionState {
    TRANSACTION_STATE_TRYING, 
    TRANSACTION_STATE_PROCEEDING,
    TRANSACTION_STATE_COMPLETED,
    TRANSACTION_STATE_TERMINATED,
    TRANSACTION_STATE_MAX,
};

enum TransactionEvent {
    TRANSACTION_EVENT_200OK,
    TRANSACTION_EVENT_100TRYING,
    TRANSACTION_EVENT_TIMER_E_FIRED,
    TRANSACTION_EVENT_TIMER_F_FIRED,
    TRANSACTION_EVENT_TIMER_K_FIRED,
    TRANSACTION_EVENT_MAX,
};

struct Transaction *CreateTransaction(struct Message *request);
void DestoryTransaction(struct Transaction **t);
void TransactionSetTimer(TransactionTimerAdder adder);

enum TransactionState TransactionGetState(struct Transaction *t);
struct Message * TransactionGetRequest(struct Transaction *t);
void TransactionAddResponse(struct Transaction *t, struct Message *message);
void RunFSM(struct Transaction *t, enum TransactionEvent event);
void TransactionSetNotifyInterface(struct Transaction *t, struct TransactionNotifyInterface *interface);
