#include "SipMethod.h"

#define NULL_DIALOG_ID NULL

struct DialogId;
struct Dialog;

struct DialogId *DialogGetId(struct Dialog *dialog);
struct UserAgent *DialogGetUserAgent(struct Dialog *dialog);
struct Message *DialogGetRequest(struct Dialog *dialog);
char *DialogGetToUser(struct Dialog *dialog);
void DialogSetRequestMethod(struct Dialog *dialog, SIP_METHOD method);
SIP_METHOD DialogGetRequestMethod(struct Dialog *dialog);
void DialogSetToUser(struct Dialog *dialog, char *to);

struct Dialog *CreateDialog(struct DialogId *dialogid, struct UserAgent *ua);
void DestoryDialog(struct Dialog **dialog);
