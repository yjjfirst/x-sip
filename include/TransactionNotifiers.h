struct Transaction;

struct TransactionManagerObserver  {
    void (*die)(struct Transaction *t);
};

struct TransactionUserObserver {
    void (*onEvent) (struct Transaction *t);
};
