#include <stdio.h>

#include "Utils.h"
#include "Dialog.h"
#include "DialogId.h"
#include "UserAgent.h"
#include "Accounts.h"
#include "TransactionManager.h"
#include "Messages.h"
#include "Transaction.h"
#include "MessageBuilder.h"
#include "Transporter.h"
#include "Header.h"
#include "URI.h"
#include "ContactHeader.h"
#include "Session.h"
#include "CallEvents.h"

struct Dialog {
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

void DialogSetLocalTag(struct Dialog *dialog, const char *localTag)
{
    struct DialogId *id = DialogGetId(dialog);
    DialogIdSetLocalTag(id, (char *)localTag);
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

void ExtractDialogIdFromMessage(struct Dialog *dialog, struct Message *message)
{
    struct DialogId *dialogid = DialogGetId(dialog);
    DialogIdExtractFromMessage(dialogid, message);                    
}

void ExtractRemoteTargetFromMessage(struct Dialog *dialog, struct Message *message)
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

void ClientInviteOkReceived(struct Dialog *dialog, struct Message *message)
{
    ExtractDialogIdFromMessage(dialog, message);
    ExtractRemoteTargetFromMessage(dialog, message);

    DialogSetState(dialog, DIALOG_STATE_CONFIRMED);
    DialogAck(dialog);

    if (NotifyClient != NULL)
        NotifyClient(CALL_ESTABLISHED, DialogGetUserAgent(dialog));

    dialog->session = CreateSession();
}

void ClientInviteRingingReceived(struct Dialog *dialog, struct Message *message)
{
    if (NotifyClient != NULL)
        NotifyClient(CALL_REMOTE_RINGING, DialogGetUserAgent(dialog));
}

void HandleClientInviteEvent(struct Transaction *t)
{
    struct Message *message = TransactionGetLatestResponse(t);
    struct Dialog *dialog = (struct Dialog *) TransactionGetUser(t);
    enum TransactionEvent event = TransactionGetCurrentEvent(t);
    
    if (event == TRANSACTION_EVENT_200OK) {
        ClientInviteOkReceived(dialog, message);
    } else if (event == TRANSACTION_EVENT_180RINGING) {
        ClientInviteRingingReceived(dialog, message);
    }
}

void HandleRegisterEvent (struct Transaction *t)
{
    struct Message *message = TransactionGetLatestResponse(t);
    struct Dialog *dialog = (struct Dialog *) TransactionGetUser(t);
    struct UserAgent *ua = DialogGetUserAgent(dialog);

    if (TransactionGetCurrentEvent(t) == TRANSACTION_EVENT_200OK) {    
        if (MessageGetExpires(message) != 0) {
            UserAgentSetBinded(ua);
        } else {
            UserAgentSetUnbinded(ua);
        }
    }
}

void HandleByeEvent(struct Transaction *t)
{
    struct Dialog *dialog = (struct Dialog *) TransactionGetUser(t);
    struct UserAgent *ua = DialogGetUserAgent(dialog);

    UserAgentRemoveDialog(ua, DialogGetId(dialog));
}

void HandleClientNonInviteEvent(struct Transaction *t)
{
    
    if (MessageGetMethod(TransactionGetRequest(t)) == SIP_METHOD_REGISTER) {
        HandleRegisterEvent(t);
    } else if (MessageGetMethod(TransactionGetRequest(t)) == SIP_METHOD_BYE) {
        HandleByeEvent(t);
    } 
    
}

void OnTransactionEventImpl(struct Transaction *t)
{
    enum TransactionType type = TransactionGetType(t);

    if ( type == TRANSACTION_TYPE_CLIENT_NON_INVITE) {
        HandleClientNonInviteEvent(t);
    } else if (type == TRANSACTION_TYPE_CLIENT_INVITE){
        HandleClientInviteEvent(t);
    } 
}

void (*OnTransactionEvent)(struct Transaction *t) = OnTransactionEventImpl;

struct Transaction *DialogAddClientInviteTransaction(struct Dialog *dialog, struct Message *message)
{
    struct DialogId *id = DialogGetId(dialog);
    struct Transaction *t = NULL;

    DialogIdSetLocalTag(id, MessageGetFromTag(message));
    DialogIdSetCallId(id, MessageGetCallId(message));

    t = AddClientInviteTransaction(message, (struct TransactionUser *)dialog);
    dialog->transaction = t;
    dialog->localSeqNumber = MessageGetCSeqNumber(message);

    return t;
}

struct Transaction *DialogAddClientNonInviteTransaction(struct Dialog *dialog, struct Message *message)
{
    return AddClientNonInviteTransaction(message, (struct TransactionUser *)dialog);
}

struct Transaction *DialogAddServerNonInviteTransaction(struct Dialog *dialog, struct Message *message)
{
    struct Transaction *t;

    t = AddServerNonInviteTransaction(message, (struct TransactionUser *)dialog);
    dialog->transaction = t;
    
    return t;
}

struct Transaction *DialogAddServerInviteTransaction(struct Dialog *dialog, struct Message *message)
{
    struct DialogId *id = DialogGetId(dialog);
    struct Transaction *t = NULL;
   
    DialogIdSetRemoteTag(id, MessageGetFromTag(message));
    DialogIdSetCallId(id, MessageGetCallId(message));
    ExtractRemoteTargetFromMessage(dialog, message);

    t = AddServerInviteTransaction(message, (struct TransactionUser *)dialog);;
    dialog->transaction = t;
    
    return t;
}

void DialogSend200OKResponse(struct Dialog *dialog)
{
    struct DialogId *id = DialogGetId(dialog);
    struct Message *message = Build200OkMessage(TransactionGetRequest(dialog->transaction));

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

void DialogInvite(struct Dialog *dialog)
{
    struct Message *invite = BuildInviteMessage(dialog);
    AddClientInviteTransaction(invite, (struct TransactionUser *)dialog);
}

void DialogBye(struct Dialog *dialog)
{
    struct Message *bye = BuildByeMessage(dialog);
    AddClientNonInviteTransaction(bye, (struct TransactionUser *)dialog);
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
