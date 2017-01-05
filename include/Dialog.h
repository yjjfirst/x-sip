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


struct URI *DialogSetRemoteUri(struct Dialog *dialog, char *to);
struct URI *DialogGetRemoteUri(struct Dialog *dialog);
struct URI *DialogGetRemoteTarget(struct Dialog *dialog);
struct Message *DialogBuildInvite(struct Dialog *dialog, char *to);


char *DialogGetRemoteTag(struct Dialog *dialog);
char *DialogGetLocalTag(struct Dialog *dialog);
void DialogSetLocalTag(struct Dialog *dialog, const char *localTag);
char *DialogGetCallId(struct Dialog *dialog);


struct DialogId *GetDialogId(struct Dialog *dialog);
struct UserAgent *DialogGetUserAgent(struct Dialog *dialog);
struct Message *DialogGetRequest(struct Dialog *dialog);

unsigned int GetLocalSeqNumber(struct Dialog *dialog);
void SetLocalSeqNumber(struct Dialog *dialog, int seq);

unsigned int GetRemoteSeqNumber(struct Dialog *dialog);

enum DIALOG_STATE GetDialogState(struct Dialog *dialog);
void SetDialogState(struct Dialog *dialog, enum DIALOG_STATE state);

void DialogSetRequestMethod(struct Dialog *dialog, SIP_METHOD method);
SIP_METHOD DialogGetRequestMethod(struct Dialog *dialog);

struct Dialog *CreateDialog(struct DialogId *dialogid, struct UserAgent *ua);
void DestroyDialog(struct Dialog **dialog);

struct Transaction *DialogNewTransaction(struct Dialog *dialog, struct Message *message, int type);

extern void (*DialogTerminate)(struct Dialog *dialog);
void DialogInvite(struct Dialog *dialog);
void DialogOk(struct Dialog *dialog);
extern void (*DialogRinging)(struct Dialog *dialog);

void DialogReceiveCancel(struct Dialog *diaog, struct Message *cancel);
void DialogReceiveBye(struct Dialog *dialog, struct Message *message);
void InviteDialogReceiveOk(struct Dialog *dialog, struct Message *message);

void DumpDialog(struct Dialog *dialog);
