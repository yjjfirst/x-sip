struct Transaction;

struct TransactionManagerNotifiers  {
    void (*die)(struct Transaction *t);
};

struct TransactionUserNotifiers {
    void (*onEvent) (struct Transaction *t);
};
