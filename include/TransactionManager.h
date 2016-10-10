#include "../utils/list/include/list.h"

struct Message;
struct UserAgent;
struct TransactionUser;


void ClearTransactionManager();
int SipMessageHandle(char *string);
struct TransactionId *TransactionIdExtract(struct TransactionId *tid, struct Message *message);

struct Transaction *GetTransaction(char *branch, char *seqMethod);
int CountTransaction(void);

struct Transaction *AddTransaction(struct Message *message, struct TransactionUser *user, int type);
void RemoveTransaction(struct Transaction *t);
void RemoveTransactionById(struct TransactionId *id);

void TransactionManagerDump();
