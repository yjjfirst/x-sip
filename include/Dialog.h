#include "SipMethod.h"

#define NULL_DIALOG_ID NULL
#define EMPTY_DIALOG_SEQNUMBER 0

struct DialogId;
struct Dialog;

enum DIALOG_STATE {
    DIALOG_STATE_EARLY,
    DIALOG_STATE_CONFIRMED,
    DIALOG_STATE_TERMINATED,
};

extern struct URI *(*DialogGetRemoteUri)(struct Dialog *dialog);

struct DialogId *DialogGetId(struct Dialog *dialog);
struct UserAgent *DialogGetUserAgent(struct Dialog *dialog);
struct Message *DialogGetRequest(struct Dialog *dialog);
unsigned int DialogGetLocalSeqNumber(struct Dialog *dialog);
unsigned int DialogGetRemoteSeqNumber(struct Dialog *dialog);
enum DIALOG_STATE DialogGetState(struct Dialog *dialog);

void DialogSetRequestMethod(struct Dialog *dialog, SIP_METHOD method);
SIP_METHOD DialogGetRequestMethod(struct Dialog *dialog);

char *DialogGetToUser(struct Dialog *dialog);
void DialogSetToUser(struct Dialog *dialog, char *to);

struct Dialog *CreateDialog(struct DialogId *dialogid, struct UserAgent *ua);
void DestoryDialog(struct Dialog **dialog);
void DialogTerminate(struct Dialog *dialog);


struct Transaction *DialogAddClientNonInviteTransaction(struct Dialog *dialog, struct Message *message);
struct Transaction *DialogAddClientInviteTransaction(struct Dialog *dialog, struct Message *message);
struct Transaction *DialogAddServerTransaction(struct Dialog *dialog, struct Message *message);
void DialogSend200OKResponse(struct Dialog *dialog);

