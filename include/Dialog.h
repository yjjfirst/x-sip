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

struct URI *DialogGetRemoteUri(struct Dialog *dialog);
struct URI *DialogGetRemoteTarget(struct Dialog *dialog);

char *DialogGetRemoteTag(struct Dialog *dialog);
char *DialogGetLocalTag(struct Dialog *dialog);
void DialogSetLocalTag(struct Dialog *dialog, const char *localTag);
char *DialogGetCallId(struct Dialog *dialog);


struct DialogId *DialogGetId(struct Dialog *dialog);
struct UserAgent *DialogGetUserAgent(struct Dialog *dialog);
struct Message *DialogGetRequest(struct Dialog *dialog);
unsigned int DialogGetLocalSeqNumber(struct Dialog *dialog);
void SetLocalSeqNumber(struct Dialog *dialog, int seq);
unsigned int DialogGetRemoteSeqNumber(struct Dialog *dialog);
enum DIALOG_STATE DialogGetState(struct Dialog *dialog);

void DialogSetRequestMethod(struct Dialog *dialog, SIP_METHOD method);
SIP_METHOD DialogGetRequestMethod(struct Dialog *dialog);

struct Dialog *CreateDialog(struct DialogId *dialogid, struct UserAgent *ua);
void DestroyDialog(struct Dialog **dialog);

struct Transaction *DialogNewTransaction(struct Dialog *dialog, struct Message *message, int type);

void DialogTerminate(struct Dialog *dialog);
void DialogInvite(struct Dialog *dialog);
void DialogBye(struct Dialog *dialog);
void DialogOk(struct Dialog *dialog);

void DialogReceiveBye(struct Dialog *dialog, struct Message *message);
void DialogReceiveOk(struct Dialog *dialog, struct Message *message);
