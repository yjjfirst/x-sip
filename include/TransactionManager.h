#include "../utils/list/include/list.h"

struct Message;
struct UserAgent;
struct TransactionOwnerInterface;

struct TransactionManager *GetTransactionManager();
void DestoryTransactionManager();
int MessageReceived(char *string);
struct Transaction *GetTransactionBy(char *branch, char *seqMethod);
int CountTransaction(void);
struct Transaction *CreateTransactionExt(struct Message *message, struct TransactionOwnerInterface *ua);
void RemoveTransaction(struct Transaction *t);
