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
    struct TransactionUserNotifiers notifyInterface;  //must be the first field in the struct.
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


void DialogHandleInviteClientEvent(struct Transaction *t)
{
    struct Message *message = TransactionGetLatestResponse(t);
    struct Dialog *dialog = (struct Dialog *) TransactionGetUser(t);

    if (TransactionGetCurrentEvent(t) == TRANSACTION_EVENT_200OK_RECEIVED) {
        struct DialogId *dialogid = DialogGetId(dialog);
        DialogIdExtractFromMessage(dialogid, message);            
            
        struct Message *ack = BuildAckMessage(dialog);
        AddClientTransaction(ack, (struct TransactionUserNotifiers *)dialog);         
    }
}

void DialogHandleNonInviteClientEvent(struct Transaction *t)
{
    struct Message *message = TransactionGetLatestResponse(t);
    struct Dialog *dialog = (struct Dialog *) TransactionGetUser(t);
    struct UserAgent *ua = DialogGetUserAgent(dialog);

    if (TransactionGetCurrentEvent(t) == TRANSACTION_EVENT_200OK_RECEIVED) {
        if (MessageGetExpires(message) != 0) {
            UserAgentSetBinded(ua);
        } else {
            UserAgentSetUnbinded(ua);
        }
    }
}

void DialogOnTransactionEvent(struct Transaction *t)
{
    enum TransactionType type = TransactionGetType(t);

    if ( type == TRANSACTION_TYPE_CLIENT_NON_INVITE) {
        DialogHandleNonInviteClientEvent(t);
    } else if (type == TRANSACTION_TYPE_CLIENT_INVITE){
        DialogHandleInviteClientEvent(t);
    } 
}

struct Transaction *DialogAddClientTransaction(struct Dialog *dialog, struct Message *message)
{
    struct DialogId *id = DialogGetId(dialog);
    struct Transaction *t = NULL;

    DialogIdSetLocalTag(id, MessageGetFromTag(message));
    DialogIdSetCallId(id, MessageGetCallId(message));
    t = AddClientTransaction(message, (struct TransactionUserNotifiers *)dialog);
    dialog->transaction = t;

    return t;
}

struct Transaction *DialogAddServerTransaction(struct Dialog *dialog, struct Message *message)
{
    struct DialogId *id = DialogGetId(dialog);
    struct Transaction *t = NULL;
   
    DialogIdSetRemoteTag(id, MessageGetFromTag(message));
    DialogIdSetCallId(id, MessageGetCallId(message));
    t = AddServerTransaction(message, (struct TransactionUserNotifiers *)dialog);;
    dialog->transaction = t;
    
    return t;
}

void DialogSend200OKResponse(struct Dialog *dialog)
{
    struct DialogId *id = DialogGetId(dialog);
    struct Message *message = Build200OKMessage(TransactionGetRequest(dialog->transaction));
    
    TransactionAddResponse(dialog->transaction, message);
    DialogIdSetLocalTag(id, MessageGetToTag(message));
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
