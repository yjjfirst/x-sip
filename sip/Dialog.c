#include <stdio.h>

#include "Utils.h"
#include "Dialog.h"
#include "DialogId.h"
#include "UserAgent.h"
#include "UserAgentManager.h"
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

struct TransactionEventAction {
    SIP_METHOD requestMethod;
    void (*action)(struct Dialog *dialog, int event, MESSAGE *message);
};

URI *DialogGetRemoteUri(struct Dialog *dialog)
{
    assert(dialog != 0);
    
    return dialog->remoteUri;
}

char *DialogGetCallId(struct Dialog *dialog)
{
    return GetCallId(dialog->id);
}

char *DialogGetRemoteTag(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return GetRemoteTag(dialog->id);
}

char *DialogGetLocalTag(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return GetLocalTag(dialog->id);
}

URI *DialogGetRemoteTarget(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return dialog->remoteTarget;
}

struct DialogId *GetDialogId(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return dialog->id;
}

struct UserAgent *DialogGetUa(struct Dialog *dialog)
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

    struct Transaction *t = dialog->transaction;
    struct Message *request = GetTransactionRequest(t);

    return MessageGetMethod(request);
}

unsigned int GetLocalSeqNumber(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return dialog->localSeqNumber;
}

void SetLocalSeqNumber(struct Dialog *dialog, int seq)
{
    assert(dialog != NULL);
    dialog->localSeqNumber = seq;
}

void DialogSetLocalTag(struct Dialog *dialog, const char *localTag)
{
    struct DialogId *id = GetDialogId(dialog);
    SetLocalTag(id, (char *)localTag);
}

unsigned int GetRemoteSeqNumber(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return dialog->remoteSeqNumber;
}

enum DIALOG_STATE GetDialogState(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return dialog->state;
}

void SetDialogState(struct Dialog *dialog, enum DIALOG_STATE state)
{
    assert(dialog != NULL);
    dialog->state = state;
}

URI *DialogSetRemoteUri(struct Dialog *dialog, char *to)
{
    struct UserAgent *ua = DialogGetUa(dialog);

    if (dialog->remoteUri != NULL) return NULL;
    
    URI *uri = CreateUri(URI_SCHEME_SIP, to, UaGetProxy(ua), 0);
    dialog->remoteUri = uri;

    return uri;
}

void ExtractRemoteTarget(struct Dialog *dialog, MESSAGE *message)
{
    CONTACT_HEADER *c = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_CONTACT, message);
    URI *uri = ContactHeaderGetUri(c);
    dialog->remoteTarget = UriDup(uri);
}

void DialogAck(struct Dialog *dialog)
{
    struct UserAgent *ua = DialogGetUa(dialog);

    MESSAGE *ack = BuildAckMessage(GetTransactionRequest(dialog->transaction),
                                   UaGetProxy(ua),
                                   UaGetProxyPort(ua));

    MessageSetToTag(ack, DialogGetRemoteTag(dialog));    
    SendMessage(ack);

    DestroyMessage(&ack);
}

void DialogReceiveOk(struct Dialog *dialog, MESSAGE *message)
{
    struct DialogId *dialogid = GetDialogId(dialog);

    ExtractDialogId(dialogid, message);                    
    ExtractRemoteTarget(dialog, message);
    SetDialogState(dialog, DIALOG_STATE_CONFIRMED);

    DialogAck(dialog);

    if (NotifyCm != NULL)
        NotifyCm(CALL_ESTABLISHED, DialogGetUa(dialog));

    dialog->session = CreateSession();
}

void DialogReceiveRinging(struct Dialog *dialog, MESSAGE *message)
{
    if (NotifyCm != NULL)
        NotifyCm(CALL_REMOTE_RINGING, DialogGetUa(dialog));
}

void HandleRegisterEvent (struct Dialog *dialog, int event, MESSAGE *message)
{
    struct UserAgent *ua = DialogGetUa(dialog);
    
    if (event == TRANSACTION_EVENT_2XX) {    
        if (MessageGetExpires(message) != 0) {
            UaSetBinded(ua);
        } else {
            UaSetUnbinded(ua);
        }
    } else if (event == TRANSACTION_EVENT_4XX) {
        MESSAGE *authMessage = BuildAuthorizationMessage(
            message,
            UaGetAuthname(ua),
            UaGetPassword(ua),
            UaGetProxy(ua),
            UaGetProxyPort(ua));
        DialogNewTransaction(dialog, authMessage, TRANSACTION_TYPE_CLIENT_NON_INVITE);
    }
}

void HandleInviteEvent(struct Dialog *dialog, int event, struct Message *message)
{
    if (event == TRANSACTION_EVENT_2XX) {
        DialogReceiveOk(dialog, message);
    }
    else if (event == TRANSACTION_EVENT_1XX) {
        DialogReceiveRinging(dialog, message);
    }
    else if (event == TRANSACTION_EVENT_NEW) {
        if (MatchMessage2Dialog(message) == NULL) {    
            struct UserAgent *ua = PassiveAddUa(message);
            struct Dialog *d = AddDialog(NULL, ua);
            DialogNewTransaction(d, message, TRANSACTION_TYPE_SERVER_INVITE);             
        }
    }
    else if (event == TRANSACTION_EVENT_CANCEL) {
            NotifyCm(CALL_PEER_CANCELED, DialogGetUa(dialog));
    }
}

void HandleByeEvent(struct Dialog *dialog, int event, MESSAGE *message)
{
    struct Dialog *matched = MatchMessage2Dialog(message);
    if (matched != NULL)
        DialogReceiveBye(matched, message);
}

void HandleCancelEvent(struct Dialog *dialog, int event, MESSAGE *message)
{
    struct Dialog *matched = MatchMessage2Dialog(message);
    if (matched != NULL)
        DialogReceiveCancel(matched, message);
}

struct TransactionEventAction TransactionEventActions[] = {
    {SIP_METHOD_REGISTER, HandleRegisterEvent},
    {SIP_METHOD_INVITE,   HandleInviteEvent},
    {SIP_METHOD_BYE,      HandleByeEvent},
    {SIP_METHOD_CANCEL,   HandleCancelEvent},
    {-1, NULL},
};

SIP_METHOD GetRequestMethod(struct Dialog *dialog, MESSAGE *message)
{
    struct Transaction *t;

    if (dialog != NULL) {
        t = dialog->transaction;
        return MessageGetMethod(GetTransactionRequest(t));
    } else {
        return MessageGetMethod(message);
    }
}

void OnTransactionEventImpl(struct Dialog *dialog,  int event, MESSAGE *message)
{
    struct TransactionEventAction *action;
    SIP_METHOD requestMethod = GetRequestMethod(dialog, message);

    action = TransactionEventActions;    
    for (; action->requestMethod != -1; action ++)
    {
        if (requestMethod == action->requestMethod) {
            return action->action(dialog, event, message);
        }
    }
}

void (*OnTransactionEvent)(struct Dialog *dialog, int event, MESSAGE *message) =
    OnTransactionEventImpl;

struct Transaction *DialogNewTransaction(struct Dialog *dialog, MESSAGE *message, int type)
{
    struct DialogId *id = GetDialogId(dialog);
    struct Transaction *t = AddTransaction(message, (struct TransactionUser *)dialog, type);
    dialog->transaction = t;
    
    if (type == TRANSACTION_TYPE_CLIENT_INVITE) {
        SetLocalTag(id, MessageGetFromTag(message));
        SetCallId(id, MessageGetCallId(message));
        SetLocalSeqNumber(dialog, MessageGetCSeqNumber(message));
    } else if (type == TRANSACTION_TYPE_SERVER_INVITE) {
        SetRemoteTag(id, MessageGetFromTag(message));
        SetCallId(id, MessageGetCallId(message));
        ExtractRemoteTarget(dialog, message);

        if (NotifyCm != NULL) {
            NotifyCm(CALL_INCOMING, DialogGetUa(dialog));
        }
    }
    
    return t;
}

void DialogOk(struct Dialog *dialog)
{
    struct DialogId *id = GetDialogId(dialog);

    MESSAGE *message = BuildResponse(GetTransactionRequest(dialog->transaction),
                                     STATUS_CODE_OK);

    dialog->remoteSeqNumber = MessageGetCSeqNumber(GetTransactionRequest(dialog->transaction));     
    if (GetDialogState(dialog) == DIALOG_STATE_NON_EXIST) {
        SetLocalTag(id, MessageGetToTag(message));
        SetDialogState(dialog, DIALOG_STATE_CONFIRMED);
        SetLocalSeqNumber(dialog, MessageGetCSeqNumber(message)); 
        dialog->session = CreateSession();

    } else if (GetDialogState(dialog) == DIALOG_STATE_CONFIRMED) {
        if (DialogGetRequestMethod(dialog) == SIP_METHOD_BYE) {
            SetDialogState(dialog, DIALOG_STATE_TERMINATED);
        }
    }

    ResponseWith(dialog->transaction, message, TRANSACTION_SEND_OK);    
}

void DialogReceiveCancel(struct Dialog *dialog, MESSAGE *cancel)
{
    DialogNewTransaction(dialog, cancel, TRANSACTION_TYPE_SERVER_NON_INVITE);
    DialogOk(dialog);
}

void DialogReceiveBye(struct Dialog *dialog, MESSAGE *bye)
{
    struct DialogId *id;
    DialogNewTransaction(dialog, bye, TRANSACTION_TYPE_SERVER_NON_INVITE);
    DialogOk(dialog);

    NotifyCm(CALL_FINISHED, dialog->ua);
    RemoveUa(dialog->ua);
    
    id = GetDialogId(dialog);
    RemoveDialogById(id);    
}

void DialogTerminateImpl(struct Dialog *dialog)
{
    struct UserAgent *ua = DialogGetUa(dialog);

    MESSAGE *bye = BuildByeMessage(
        NULL,
        UaGetProxy(ua),
        UaGetProxyPort(ua));
    DialogNewTransaction(dialog, bye, TRANSACTION_TYPE_CLIENT_NON_INVITE);
    dialog->state = DIALOG_STATE_TERMINATED;
}
void (*DialogTerminate)(struct Dialog *dialog) = DialogTerminateImpl;

void DialogRingingImpl(struct Dialog *dialog)
{
    Response(dialog->transaction, TRANSACTION_SEND_RINGING);
}
void (*DialogRinging)(struct Dialog *dialog) = DialogRingingImpl;

struct Message *DialogBuildInvite(struct Dialog *dialog, char *dest)
{
    struct UserAgent *ua = DialogGetUa(dialog);    
    struct Message *invite = BuildInviteMessage(
        UaGetUsername(ua),
        dest,
        UaGetProxy(ua),
        UaGetProxyPort(ua));    

    DialogSetRemoteUri(dialog, dest);

    return invite;
}

void DialogInvite(struct Dialog *dialog, char *dest)
{
    struct Message *invite = DialogBuildInvite(dialog, dest);
    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_CLIENT_INVITE);
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

void DumpDialog(struct Dialog *dialog)
{
    printf("Local Tag: %s | Remote Tag: %s | Dialog Id: %s\n",
           DialogGetLocalTag(dialog),
           DialogGetRemoteTag(dialog),
           DialogGetCallId(dialog));
}
