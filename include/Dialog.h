#include "SipMethod.h"

#define NULL_DIALOG_ID NULL
#define EMPTY_DIALOG_SEQNUMBER 0

struct DialogId;
struct Dialog;
struct URI;

enum DIALOG_STATE {
    DIALOG_STATE_NON_EXIST,
    DIALOG_STATE_EARLY,
    DIALOG_STATE_CONFIRMED,
    DIALOG_STATE_TERMINATED,
};


void  DialogSetRemoteUri(struct Dialog *dialog, struct URI *uri);

extern struct URI *(*DialogGetRemoteUri)(struct Dialog *dialog);
extern struct URI *(*DialogGetRemoteTarget)(struct Dialog *dialog);

extern char *(*DialogGetRemoteTag)(struct Dialog *dialog);
extern char *(*DialogGetLocalTag)(struct Dialog *dialog);
void DialogSetLocalTag(struct Dialog *dialog, const char *localTag);
extern char *(*DialogGetCallId)(struct Dialog *dialog);


struct DialogId *DialogGetId(struct Dialog *dialog);
struct UserAgent *DialogGetUserAgent(struct Dialog *dialog);
struct Message *DialogGetRequest(struct Dialog *dialog);
unsigned int DialogGetLocalSeqNumber(struct Dialog *dialog);
void DialogSetLocalSeqNumber(struct Dialog *dialog, int seq);
unsigned int DialogGetRemoteSeqNumber(struct Dialog *dialog);
enum DIALOG_STATE DialogGetState(struct Dialog *dialog);

void DialogSetRequestMethod(struct Dialog *dialog, SIP_METHOD method);
SIP_METHOD DialogGetRequestMethod(struct Dialog *dialog);

struct Dialog *CreateDialog(struct DialogId *dialogid, struct UserAgent *ua);
void DestroyDialog(struct Dialog **dialog);
void DialogTerminate(struct Dialog *dialog);
void DialogInvite(struct Dialog *dialog);
void DialogBye(struct Dialog *dialog);

struct Transaction *DialogAddClientNonInviteTransaction(struct Dialog *dialog, struct Message *message);
struct Transaction *DialogAddClientInviteTransaction(struct Dialog *dialog, struct Message *message);
struct Transaction *DialogAddServerInviteTransaction(struct Dialog *dialog, struct Message *message);
struct Transaction *DialogAddServerNonInviteTransaction(struct Dialog *dialog, struct Message *message);

void DialogSend200OKResponse(struct Dialog *dialog);
void DialogReceiveBye(struct Dialog *dialog, struct Message *message);
void ClientInviteOkReceived(struct Dialog *dialog, struct Message *message);

extern void (*OnTransactionEvent)(struct Transaction *t);
