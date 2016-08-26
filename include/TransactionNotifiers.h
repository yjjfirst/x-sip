struct Transaction;

struct TransactionManagerObserver  {
    void (*die)(struct Transaction *t);
};

struct TransactionUser {
    void (*onEvent) (struct Transaction *t);
};
