#include <stdio.h>

#include "Utils.h"
#include "Dialog.h"
#include "DialogId.h"
#include "UserAgent.h"
#include "TransactionNotifiers.h"
#include "TransactionManager.h"
#include "Messages.h"
#include "Transaction.h"
#include "MessageBuilder.h"
#include "MessageTransport.h"

struct Dialog {
    struct TransactionUserNofifiers notifyInterface;  //must be the first field in the struct.
    SIP_METHOD requestMethod;
    struct Transaction *transaction;
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
    struct Message *message = TransactionGetLatestResponse(t);
    struct Dialog *dialog = (struct Dialog *) TransactionGetOwner(t);
    struct UserAgent *ua = DialogGetUserAgent(dialog);

    if (TransactionGetCurrentEvent(t) == TRANSACTION_EVENT_200OK_RECEIVED) {
        if (TransactionGetType(t) == TRANSACTION_TYPE_CLIENT_NON_INVITE) {
            if (MessageGetExpires(message) != 0)
                UserAgentSetBinded(ua);
            else
                UserAgentSetUnbinded(ua);
        } else {
            struct DialogId *dialogid = DialogGetId(dialog);
            DialogIdExtractFromMessage(dialogid, message);            

            struct Message *ack = BuildAckMessage(dialog);
            MessageSetRemoteTag(ack, MessageGetRemoteTag(message));
            AddClientTransaction(ack, (struct TransactionUserNofifiers *)dialog);
        } 
    }
}

struct Dialog *CreateDialog(struct DialogId *dialogid, struct UserAgent *ua)
{
    struct Dialog *dialog = calloc(1, sizeof(struct Dialog));

    if (dialogid == NULL) {
        dialog->id = CreateEmptyDialogId();
    } else {
        dialog->id = dialogid;
    }
    dialog->ua = ua;
    dialog->notifyInterface.onEvent = DialogOnTransactionEvent;

    if (ua != NULL)
        UserAgentAddDialog(ua, dialog);

    return dialog;
}

void DestoryDialog(struct Dialog **dialog)
{
    if (*dialog != NULL) {
        DestoryDialogId(&(*dialog)->id);
        free(*dialog);
    }
}
