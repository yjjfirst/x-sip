#include "../utils/list/include/list.h"

struct Message;
struct UserAgent;
struct TransactionUserNotifiers;

void EmptyTransactionManager();
int MessageReceived(char *string);
struct TransactionId *ExtractTransactionIdFromMessage(struct TransactionId *tid, struct Message *message);

struct Transaction *GetTransaction(char *branch, char *seqMethod);
int CountTransaction(void);
struct Transaction *AddClientTransaction(struct Message *message, struct TransactionUserNotifiers *user);
struct Transaction *AddClientInviteTransaction(struct Message *message, struct TransactionUserNotifiers *user);
struct Transaction *AddServerTransaction(struct Message *message, struct TransactionUserNotifiers *user);

void RemoveTransaction(struct Transaction *t);










