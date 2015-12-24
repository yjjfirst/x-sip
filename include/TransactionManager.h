#include "../utils/list/include/list.h"

struct Message;
struct UserAgent;
struct TransactionOwner;

void EmptyTransactionManager();
int MessageReceived(char *string);
struct Transaction *GetTransactionBy(char *branch, char *seqMethod);
int CountTransaction(void);
struct Transaction *AddTransaction(struct Message *message, struct TransactionOwner *ua);
void RemoveTransaction(struct Transaction *t);
