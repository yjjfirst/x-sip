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
#include "DialogManager.h"

struct Dialog {
    struct Transaction *transaction;
    struct DialogId *id;
    struct UserAgent *ua;
    struct Session *session;
    URI *remoteTarget;
    URI *remoteUri;
    
    SIP_METHOD requestMethod;
    unsigned int localSeqNumber;
    unsigned int remoteSeqNumber;
    enum DIALOG_STATE state;
};

URI *DialogGetRemoteUriImpl(struct Dialog *dialog)
{
    assert(dialog != 0);
    
    return dialog->remoteUri;
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

URI *DialogGetRemoteTargetImpl(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return dialog->remoteTarget;
}

char *(*DialogGetCallId)(struct Dialog *dialog) = DialogGetCallIdImpl;
char *(*DialogGetRemoteTag)(struct Dialog *dialog) = DialogGetRemoteTagImpl;
char *(*DialogGetLocalTag)(struct Dialog *dialog) = DialogGetLocalTagImpl;
URI *(*DialogGetRemoteUri)(struct Dialog *dialog) = DialogGetRemoteUriImpl;
URI *(*DialogGetRemoteTarget)(struct Dialog *dialog) = DialogGetRemoteTargetImpl;


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

void DialogSetLocalSeqNumber(struct Dialog *dialog, int seq)
{
    assert(dialog != NULL);
    dialog->localSeqNumber = seq;
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

void DialogSetRemoteUri(struct Dialog *dialog, URI *uri)
{
    assert(uri != NULL);
    assert(dialog != NULL);
    
    dialog->remoteUri = uri;
}

void ExtractDialogIdFromMessage(struct Dialog *dialog, MESSAGE *message)
{
    struct DialogId *dialogid = DialogGetId(dialog);
    DialogIdExtractFromMessage(dialogid, message);                    
}

void ExtractRemoteTargetFromMessage(struct Dialog *dialog, MESSAGE *message)
{
    CONTACT_HEADER *c = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_CONTACT, message);
    URI *uri = ContactHeaderGetUri(c);
    dialog->remoteTarget = UriDup(uri);
}

void DialogAck(struct Dialog *dialog)
{
    MESSAGE *ack = BuildAckMessage(dialog);
    TransactionSendMessage(ack);
    DestroyMessage(&ack);
}

void ClientReceiveOk(struct Dialog *dialog, MESSAGE *message)
{
    ExtractDialogIdFromMessage(dialog, message);
    ExtractRemoteTargetFromMessage(dialog, message);

    DialogSetState(dialog, DIALOG_STATE_CONFIRMED);
    DialogAck(dialog);

    if (NotifyClient != NULL)
        NotifyClient(CALL_ESTABLISHED, DialogGetUserAgent(dialog));

    dialog->session = CreateSession();
}

void ClientInviteRingingReceived(struct Dialog *dialog, MESSAGE *message)
{
    if (NotifyClient != NULL)
        NotifyClient(CALL_REMOTE_RINGING, DialogGetUserAgent(dialog));
}

void HandleClientInviteEvent(struct Transaction *t)
{
    MESSAGE *message = TransactionGetLatestResponse(t);
    struct Dialog *dialog = (struct Dialog *) TransactionGetUser(t);
    enum TransactionEvent event = TransactionGetCurrentEvent(t);
    
    if (event == TRANSACTION_EVENT_200OK) {
        ClientReceiveOk(dialog, message);
    } else if (event == TRANSACTION_EVENT_180RINGING) {
        ClientInviteRingingReceived(dialog, message);
    }
}

void HandleRegisterEvent (struct Transaction *t)
{
    MESSAGE *message = TransactionGetLatestResponse(t);
    struct Dialog *dialog = (struct Dialog *) TransactionGetUser(t);
    struct UserAgent *ua = DialogGetUserAgent(dialog);
    enum TransactionEvent event = TransactionGetCurrentEvent(t);
    
    if (event == TRANSACTION_EVENT_200OK) {    
        if (MessageGetExpires(message) != 0) {
            UserAgentSetBinded(ua);
        } else {
            UserAgentSetUnbinded(ua);
        }
    } else if (event == TRANSACTION_EVENT_401UNAUTHORIZED) {
        MESSAGE *authMessage = BuildAuthorizationMessage(dialog, message);
        DialogAddClientNonInviteTransaction(dialog, authMessage);
    }
}

void HandleByeEvent(struct Transaction *t)
{
    struct Dialog *dialog = (struct Dialog *) TransactionGetUser(t);
    RemoveDialog(DialogGetId(dialog));
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

struct Transaction *DialogAddClientInviteTransaction(struct Dialog *dialog, MESSAGE *message)
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

struct Transaction *DialogAddClientNonInviteTransaction(struct Dialog *dialog, MESSAGE *message)
{
    return AddClientNonInviteTransaction(message, (struct TransactionUser *)dialog);
}

struct Transaction *DialogAddServerNonInviteTransaction(struct Dialog *dialog, MESSAGE *message)
{
    struct Transaction *t;

    t = AddServerNonInviteTransaction(message, (struct TransactionUser *)dialog);
    dialog->transaction = t;
    
    return t;
}

struct Transaction *DialogAddServerInviteTransaction(struct Dialog *dialog, MESSAGE *message)
{
    struct DialogId *id = DialogGetId(dialog);
    struct Transaction *t = NULL;
   
    DialogIdSetRemoteTag(id, MessageGetFromTag(message));
    DialogIdSetCallId(id, MessageGetCallId(message));
    ExtractRemoteTargetFromMessage(dialog, message);

    if (NotifyClient != NULL) {
        NotifyClient(CALL_INCOMING, DialogGetUserAgent(dialog));
    }

    t = AddServerInviteTransaction(message, (struct TransactionUser *)dialog);
    dialog->transaction = t;
    
    
    return t;
}

void DialogOk(struct Dialog *dialog)
{
    struct DialogId *id = DialogGetId(dialog);
    MESSAGE *message = Build200OkMessage(NULL, TransactionGetRequest(dialog->transaction));

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

void DialogReceiveBye(struct Dialog *dialog, MESSAGE *bye)
{
    DialogAddServerNonInviteTransaction(dialog, bye);
    DialogOk(dialog);
}

void DialogTerminate(struct Dialog *dialog)
{
    MESSAGE *bye = BuildByeMessage(dialog);
    DialogAddClientNonInviteTransaction(dialog, bye);
    dialog->state = DIALOG_STATE_TERMINATED;
}

void DialogInvite(struct Dialog *dialog)
{
    MESSAGE *invite = BuildInviteMessage(dialog, (char *)"88002");
    AddClientInviteTransaction(invite, (struct TransactionUser *)dialog);
}

void DialogBye(struct Dialog *dialog)
{
    MESSAGE *bye = BuildByeMessage(dialog);
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

void DestroyDialog(struct Dialog **dialog)
{
    struct Dialog *d = *dialog;

    if (d != NULL) {
        DestroyDialogId(&d->id);
        DestroyUri(&d->remoteTarget);
        DestroyUri(&(d->remoteUri));
        DestroySession(&d->session);
        free(d);
        *dialog = NULL;
    }
}
