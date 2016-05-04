#include "../utils/list/include/list.h"

struct Message;
struct UserAgent;
struct TransactionUserNotifiers;

void EmptyTransactionManager();
int MessageReceived(char *string);
struct TransactionId *ExtractTransactionIdFromMessage(struct TransactionId *tid, struct Message *message);

struct Transaction *GetTransaction(char *branch, char *seqMethod);
int CountTransaction(void);
struct Transaction *AddClientNonInviteTransaction(struct Message *message, struct TransactionUserNotifiers *user);
struct Transaction *AddClientInviteTransaction(struct Message *message, struct TransactionUserNotifiers *user);
struct Transaction *AddServerInviteTransaction(struct Message *message, struct TransactionUserNotifiers *user);
struct Transaction *AddServerNonInviteTransaction(struct Message *message, struct TransactionUserNotifiers *user);


void RemoveTransaction(struct Transaction *t);










