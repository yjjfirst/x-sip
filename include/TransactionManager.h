#include "../utils/list/include/list.h"

struct Message;

struct TransactionManager {
    t_list *transactions;
    struct Transaction *(*CreateTransaction)(struct Message *message);
    int (*CountTransaction)(void);
};


struct TransactionManager *GetTransactionManager();
void DestoryTransactionManager(struct TransactionManager **manager);
int MessageReceived(char *string);
struct Transaction *GetTransactionBy(char *branch, char *seqMethod);
