#include <stdio.h>

#include "Utils.h"
#include "Dialog.h"
#include "DialogId.h"
#include "UserAgent.h"
#include "Accounts.h"
#include "TransactionNotifiers.h"
#include "TransactionManager.h"
#include "Messages.h"
#include "Transaction.h"
#include "MessageBuilder.h"
#include "MessageTransport.h"
#include "Header.h"
#include "URI.h"
#include "ContactHeader.h"

struct Dialog {
    struct TransactionUserNotifiers notifyInterface;  //must be the first field in the struct.
    SIP_METHOD requestMethod;
    struct Transaction *transaction;
    struct DialogId *id;
    unsigned int localSeqNumber;
    unsigned int remoteSeqNumber;
    struct URI *remoteTarget;
    enum DIALOG_STATE state;
    struct UserAgent *ua;
    char to[USER_NAME_MAX_LENGTH];
    
};

struct URI *DialogGetRemoteUriImpl(struct Dialog *dialog)
{
    return NULL;
}
struct URI *(*DialogGetRemoteUri)(struct Dialog *dialog) = DialogGetRemoteUriImpl;

char *DialogGetRemoteTagImpl(struct Dialog *dialog)
{
    return "";
}
char *(*DialogGetRemoteTag)(struct Dialog *dialog) = DialogGetRemoteTagImpl;

struct URI *DialogGetRemoteTargetImpl(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return dialog->remoteTarget;
}
struct URI *(*DialogGetRemoteTarget)(struct Dialog *dialog) = DialogGetRemoteTargetImpl;


struct DialogId *DialogGetId(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return dialog->id;
}

struct UserAgent *DialogGetUserAgent(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return dialog->ua;
}

DEFINE_STRING_MEMBER_WRITER(struct Dialog, DialogSetToUser, to, USER_NAME_MAX_LENGTH);
DEFINE_STRING_MEMBER_READER(struct Dialog, DialogGetToUser, to);

void DialogSetRequestMethod(struct Dialog *dialog, SIP_METHOD method)
{
    assert(dialog != NULL);
    dialog->requestMethod = method;
}


SIP_METHOD DialogGetRequestMethod(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return dialog->requestMethod;
}

unsigned int DialogGetLocalSeqNumber(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return dialog->localSeqNumber;
}

unsigned int DialogGetRemoteSeqNumber(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return dialog->remoteSeqNumber;
}

enum DIALOG_STATE DialogGetState(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return dialog->state;
}

void DialogSetState(struct Dialog *dialog, enum DIALOG_STATE state)
{
    assert(dialog != NULL);
    dialog->state = state;
}

void DialogExtractDialogIdFromMessage(struct Dialog *dialog, struct Message *message)
{
    struct DialogId *dialogid = DialogGetId(dialog);
    DialogIdExtractFromMessage(dialogid, message);                    
}

void DialogExtractRemoteTargetFromMessage(struct Dialog *dialog, struct Message *message)
{
    struct ContactHeader *c = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_CONTACT, message);
    struct URI *uri = ContactHeaderGetUri(c);
    dialog->remoteTarget = UriDup(uri);
}

void DialogAck(struct Dialog *dialog)
{
    struct Message *ack = BuildAckMessage(dialog);
    AddClientNonInviteTransaction(ack, (struct TransactionUserNotifiers *)dialog);         
}

void DialogHandleInviteClientEvent(struct Transaction *t)
{
    struct Message *message = TransactionGetLatestResponse(t);
    struct Dialog *dialog = (struct Dialog *) TransactionGetUser(t);
    
    if (TransactionGetCurrentEvent(t) == TRANSACTION_EVENT_200OK_RECEIVED) {
        DialogExtractDialogIdFromMessage(dialog, message);
        DialogSetState(dialog, DIALOG_STATE_CONFIRMED);
        DialogExtractRemoteTargetFromMessage(dialog, message);
        DialogAck(dialog);
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

struct Transaction *DialogAddClientInviteTransaction(struct Dialog *dialog, struct Message *message)
{
    struct DialogId *id = DialogGetId(dialog);
    struct Transaction *t = NULL;

    DialogIdSetLocalTag(id, MessageGetFromTag(message));
    DialogIdSetCallId(id, MessageGetCallId(message));

    t = AddClientInviteTransaction(message, (struct TransactionUserNotifiers *)dialog);
    dialog->transaction = t;
    dialog->localSeqNumber = MessageGetCSeqNumber(message);

    return t;
}

struct Transaction *DialogAddClientNonInviteTransaction(struct Dialog *dialog, struct Message *message)
{
    return AddClientNonInviteTransaction(message, (struct TransactionUserNotifiers *)dialog);
}

struct Transaction *DialogAddServerTransaction(struct Dialog *dialog, struct Message *message)
{
    struct DialogId *id = DialogGetId(dialog);
    struct Transaction *t = NULL;
   
    DialogIdSetRemoteTag(id, MessageGetFromTag(message));
    DialogIdSetCallId(id, MessageGetCallId(message));
    DialogExtractRemoteTargetFromMessage(dialog, message);
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
    DialogSetState(dialog, DIALOG_STATE_CONFIRMED);
    dialog->remoteSeqNumber = MessageGetCSeqNumber(TransactionGetRequest(dialog->transaction));
}

void DialogTerminate(struct Dialog *dialog)
{
    struct Message *bye = BuildByeMessage(dialog);
    DialogAddClientNonInviteTransaction(dialog, bye);
    dialog->state = DIALOG_STATE_TERMINATED;
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
        DestoryUri((*dialog)->remoteTarget);
        free(*dialog);
    }
}
