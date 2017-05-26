#include "../utils/list/include/list.h"

struct Message;
struct UserAgent;
struct TransactionUser;

void ClearTransactionManager();
int SipMessageCallback(char *string, char *ipAddr, int port);
BOOL SipMessageInput(struct Message *message);

struct TransactionId *ExtractTransactionId(struct TransactionId *tid, struct Message *message);

struct Transaction *GetTransaction(char *branch, char *seqMethod);
int CountTransaction(void);

extern struct Transaction *(*AddTransaction)(struct Message *message, struct TransactionUser *user, int type);
void RemoveTransaction(struct Transaction *t);
void RemoveTransactionById(struct TransactionId *id);
struct Transaction *MatchResponse(struct Message *message);
struct Transaction *MatchRequest(struct Message *message);
void RunFsmByStatusCode(struct Transaction *t, int statusCode);

void DumpTransactionManager();
