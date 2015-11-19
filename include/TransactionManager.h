#include "../utils/list/include/list.h"

struct Message;

struct TransactionManager *GetTransactionManager();
void DestoryTransactionManager(struct TransactionManager **manager);
int MessageReceived(char *string);
struct Transaction *GetTransactionBy(char *branch, char *seqMethod);
int CountTransaction(void);
struct Transaction *CreateTransactionExt(struct Message *message);
