struct Transaction;

struct TransactionManagerNotifiers  {
    void (*die)(struct Transaction *t);
};

struct TransactionUserNofifiers {
    void (*onEvent) (struct Transaction *t);
};
