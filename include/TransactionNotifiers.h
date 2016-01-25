struct Transaction;

struct TransactionManagerNotifiers  {
    void (*die)(struct Transaction *t);
};

struct TransactionUser {
    void (*onEvent) (struct Transaction *t);
};
