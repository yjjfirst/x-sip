struct Transaction;

struct TransactionNotifiers  {
    void (*die)(struct Transaction *t);
};

struct TransactionOwner {
    void (*onEvent) (struct Transaction *t);
};
