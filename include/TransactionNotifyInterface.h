struct Transaction;

struct TransactionManagerInterface  {
    void (*die)(struct Transaction *t);
};

struct TransactionOwnerInterface {
    void (*onEvent) (struct Transaction *t);
};
