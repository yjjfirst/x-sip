#include "Utils.h"
#include "Dialog.h"
#include "DialogId.h"
#include "UserAgent.h"
#include "TransactionNotifyInterface.h"
#include "Messages.h"
#include "Transaction.h"

struct Dialog {
    struct TransactionOwnerInterface notifyInterface;  //must be the first field in the struct.
    SIP_METHOD requestMethod;
    struct DialogId *id;
    struct UserAgent *ua;
    char to[USER_NAME_MAX_LENGTH];
};

struct DialogId *DialogGetId(struct Dialog *dialog)
{
    return dialog->id;
}

struct UserAgent *DialogGetUserAgent(struct Dialog *dialog)
{
    return dialog->ua;
}

DEFINE_STRING_MEMBER_WRITER(struct Dialog, DialogSetToUser, to, USER_NAME_MAX_LENGTH);
DEFINE_STRING_MEMBER_READER(struct Dialog, DialogGetToUser, to);

void DialogSetRequestMethod(struct Dialog *dialog, SIP_METHOD method)
{
    dialog->requestMethod = method;
}

SIP_METHOD DialogGetRequestMethod(struct Dialog *dialog)
{
    return dialog->requestMethod;
}

void DialogOnTransactionEvent(struct Transaction *t)
{
    struct Message *m = TransactionGetLatestResponse(t);
    struct Dialog *dialog = (struct Dialog *) TransactionGetOwner(t);
    struct UserAgent *ua = DialogGetUserAgent(dialog);

    if (TransactionGetCurrentEvent(t) == TRANSACTION_EVENT_200OK) {
        if (TransactionGetType(t) == TRANSACTION_TYPE_CLIENT_NON_INVITE) {
            if (MessageGetExpires(m) != 0)
                UserAgentSetBinded(ua);
            else
                UserAgentSetUnbinded(ua);
        } else {
            struct DialogId *dialogid = CreateDialogIdFromMessage(m);
            CreateDialog(dialogid, ua);
        } 
    }
}

struct Dialog *CreateDialog(struct DialogId *dialogid, struct UserAgent *ua)
{
    struct Dialog *dialog = calloc(1, sizeof(struct Dialog));
    dialog->id = dialogid;
    dialog->ua = ua;
    dialog->notifyInterface.onEvent = DialogOnTransactionEvent;
    UserAgentAddDialog(ua, dialog);
    DialogSetToUser(dialog, "88002");

    return dialog;
}

void DestoryDialog(struct Dialog **dialog)
{
    if (*dialog != NULL) {
        DestoryDialogId(&(*dialog)->id);
        free(*dialog);
    }
}
