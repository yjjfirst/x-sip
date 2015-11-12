struct Message;
struct Transaction;

typedef void (*TimerCallback)(void *transaction);
typedef void (*TransactionTimerAdder)(void *transaction, int ms, TimerCallback onTime);
typedef void (*TransactionAction)(struct Transaction *t);

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
    TRANSACTION_EVENT_MAX,
};

struct Transaction *CreateTransaction(struct Message *request);
void DestoryTransaction(struct Transaction **t);
enum TransactionState TransactionGetState(struct Transaction *t);
int TransactionHandleMessage(char *string);
void TransactionSetTimerAdder(TransactionTimerAdder adder);
