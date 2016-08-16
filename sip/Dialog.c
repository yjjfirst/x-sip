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
#include "Session.h"

struct Dialog {
    struct TransactionUserObserver userOberver;  //must be the first field in the struct.
    struct Transaction *transaction;
    struct DialogId *id;
    struct UserAgent *ua;
    struct Session *session;
    struct URI *remoteTarget;

    SIP_METHOD requestMethod;
    unsigned int localSeqNumber;
    unsigned int remoteSeqNumber;
    enum DIALOG_STATE state;
    char to[USER_NAME_MAX_LENGTH];
};

struct URI *DialogGetRemoteUriImpl(struct Dialog *dialog)
{
    return NULL;
}

char *DialogGetCallIdImpl(struct Dialog *dialog)
{
    return DialogIdGetCallId(dialog->id);
}

char *DialogGetRemoteTagImpl(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return DialogIdGetRemoteTag(dialog->id);
}

char *DialogGetLocalTagImpl(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return DialogIdGetLocalTag(dialog->id);
}

struct URI *DialogGetRemoteTargetImpl(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return dialog->remoteTarget;
}

char *(*DialogGetCallId)(struct Dialog *dialog) = DialogGetCallIdImpl;
char *(*DialogGetRemoteTag)(struct Dialog *dialog) = DialogGetRemoteTagImpl;
char *(*DialogGetLocalTag)(struct Dialog *dialog) = DialogGetLocalTagImpl;
struct URI *(*DialogGetRemoteUri)(struct Dialog *dialog) = DialogGetRemoteUriImpl;
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
    TransactionSendMessage(ack);
    DestroyMessage(&ack);
}

void DialogClientInviteOkReceived(struct Dialog *dialog, struct Message *message)
{
    DialogExtractDialogIdFromMessage(dialog, message);
    DialogSetState(dialog, DIALOG_STATE_CONFIRMED);
    DialogExtractRemoteTargetFromMessage(dialog, message);
    DialogAck(dialog);

    dialog->session = CreateSession();
}

void HandleClientInviteEvent(struct Transaction *t)
{
    struct Message *message = TransactionGetLatestResponse(t);
    struct Dialog *dialog = (struct Dialog *) TransactionGetUser(t);
    
    if (TransactionGetCurrentEvent(t) == TRANSACTION_EVENT_200OK_RECEIVED) {
        DialogClientInviteOkReceived(dialog, message);
    }
}

void HandleClientNonInviteEvent(struct Transaction *t)
{
    struct Message *message = TransactionGetLatestResponse(t);
    struct Dialog *dialog = (struct Dialog *) TransactionGetUser(t);
    struct UserAgent *ua = DialogGetUserAgent(dialog);
    
    if (MessageGetMethod(TransactionGetRequest(t)) == SIP_METHOD_REGISTER) {
        if (TransactionGetCurrentEvent(t) == TRANSACTION_EVENT_200OK_RECEIVED) {
            if (MessageGetExpires(message) != 0) {
                UserAgentSetBinded(ua);
            } else {
                UserAgentSetUnbinded(ua);
            }
        }
    } else if (MessageGetMethod(TransactionGetRequest(t)) == SIP_METHOD_BYE) {
        UserAgentRemoveDialog(ua, DialogGetId(dialog));
    }
    
}

void OnTransactionEvent(struct Transaction *t)
{
    enum TransactionType type = TransactionGetType(t);

    if ( type == TRANSACTION_TYPE_CLIENT_NON_INVITE) {
        HandleClientNonInviteEvent(t);
    } else if (type == TRANSACTION_TYPE_CLIENT_INVITE){
        HandleClientInviteEvent(t);
    } 
}

struct Transaction *DialogAddClientInviteTransaction(struct Dialog *dialog, struct Message *message)
{
    struct DialogId *id = DialogGetId(dialog);
    struct Transaction *t = NULL;

    DialogIdSetLocalTag(id, MessageGetFromTag(message));
    DialogIdSetCallId(id, MessageGetCallId(message));

    t = AddClientInviteTransaction(message, (struct TransactionUserObserver *)dialog);
    dialog->transaction = t;
    dialog->localSeqNumber = MessageGetCSeqNumber(message);

    return t;
}

struct Transaction *DialogAddClientNonInviteTransaction(struct Dialog *dialog, struct Message *message)
{
    return AddClientNonInviteTransaction(message, (struct TransactionUserObserver *)dialog);
}

struct Transaction *DialogAddServerNonInviteTransaction(struct Dialog *dialog, struct Message *message)
{
    struct Transaction *t;

    t = AddServerNonInviteTransaction(message, (struct TransactionUserObserver *)dialog);
    dialog->transaction = t;
    
    return t;
}

struct Transaction *DialogAddServerInviteTransaction(struct Dialog *dialog, struct Message *message)
{
    struct DialogId *id = DialogGetId(dialog);
    struct Transaction *t = NULL;
   
    DialogIdSetRemoteTag(id, MessageGetFromTag(message));
    DialogIdSetCallId(id, MessageGetCallId(message));
    DialogExtractRemoteTargetFromMessage(dialog, message);
    t = AddServerInviteTransaction(message, (struct TransactionUserObserver *)dialog);;
    dialog->transaction = t;
    
    return t;
}

void DialogSend200OKResponse(struct Dialog *dialog)
{
    struct DialogId *id = DialogGetId(dialog);
    struct Message *message = Build200OKMessage(TransactionGetRequest(dialog->transaction));

    dialog->remoteSeqNumber = MessageGetCSeqNumber(TransactionGetRequest(dialog->transaction));     
    ResponseWith200OK(dialog->transaction);

    if (DialogGetState(dialog) == DIALOG_STATE_NON_EXIST) {        
        DialogIdSetLocalTag(id, MessageGetToTag(message));
        DialogSetState(dialog, DIALOG_STATE_CONFIRMED);
        dialog->session = CreateSession();
    } else if (DialogGetState(dialog) == DIALOG_STATE_CONFIRMED) {
        if (DialogGetRequestMethod(dialog) == SIP_METHOD_BYE) {
            DialogSetState(dialog, DIALOG_STATE_TERMINATED);
        }
    }

    DestroyMessage(&message);
}

void DialogReceiveBye(struct Dialog *dialog, struct Message *bye)
{
    DialogAddServerNonInviteTransaction(dialog, bye);
    DialogSend200OKResponse(dialog);
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
        dialog->id = CreateDialogId();
    } else {
        dialog->id = dialogid;
    }

    dialog->ua = ua;
    dialog->userOberver.onEvent = OnTransactionEvent;

    return dialog;
}

struct Dialog *AddNewDialog(struct DialogId *dialogid, struct UserAgent *ua)
{
    struct Dialog *dialog = CreateDialog(dialogid, ua);

    if (ua != NULL)
        UserAgentAddDialog(ua, dialog);

    return dialog;
}

void DestroyDialog(struct Dialog **dialog)
{
    struct Dialog *d = *dialog;
    if (d != NULL) {
        DestroyDialogId(&d->id);
        DestroyUri(&d->remoteTarget);
        DestroySession(&d->session);
        free(d);
        d = NULL;
    }
}
