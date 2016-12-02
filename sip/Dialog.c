#include <stdio.h>

#include "Utils.h"
#include "Dialog.h"
#include "DialogId.h"
#include "UserAgent.h"
#include "UserAgentManager.h"
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
#include "AccountManager.h"

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
    int event;
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
    struct UserAgent *ua = DialogGetUserAgent(dialog);
    struct Account *account = UaGetAccount(ua);    

    if (dialog->remoteUri != NULL) return NULL;
    
    URI *uri = CreateUri(URI_SCHEME_SIP, to, AccountGetProxyAddr(account), 0);
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
    MESSAGE *ack = BuildAckMessage(dialog);
    SendMessage(ack);
    DestroyMessage(&ack);
}

void InviteDialogReceiveOk(struct Dialog *dialog, MESSAGE *message)
{
    struct DialogId *dialogid = GetDialogId(dialog);

    ExtractDialogId(dialogid, message);                    
    ExtractRemoteTarget(dialog, message);
    SetDialogState(dialog, DIALOG_STATE_CONFIRMED);
    
    DialogAck(dialog);

    if (NotifyCm != NULL)
        NotifyCm(CALL_ESTABLISHED, DialogGetUserAgent(dialog));

    dialog->session = CreateSession();
}

void InviteDialogReceiveRinging(struct Dialog *dialog, MESSAGE *message)
{
    if (NotifyCm != NULL)
        NotifyCm(CALL_REMOTE_RINGING, DialogGetUserAgent(dialog));
}

void HandleRegisterEvent (struct Dialog *dialog, int event, MESSAGE *message)
{
    struct UserAgent *ua = DialogGetUserAgent(dialog);
    
    if (event == TRANSACTION_EVENT_OK) {    
        if (MessageGetExpires(message) != 0) {
            UaSetBinded(ua);
        } else {
            UaSetUnbinded(ua);
        }
    } else if (event == TRANSACTION_EVENT_UNAUTHORIZED) {
        MESSAGE *authMessage = BuildAuthorizationMessage(dialog, message);
        DialogNewTransaction(dialog, authMessage, TRANSACTION_TYPE_CLIENT_NON_INVITE);
    }
}

void HandleInviteEvent(struct Dialog *dialog, int event, struct Message *message)
{
    if (event == TRANSACTION_EVENT_OK) {
        InviteDialogReceiveOk(dialog, message);
    } else if (event == TRANSACTION_EVENT_RINGING) {
        InviteDialogReceiveRinging(dialog, message);
    }
}

void HandleNewTransactionEvent(struct Dialog *dialog, int event, MESSAGE *message)
{
    struct Dialog *matched = MatchMessage2Dialog(message);
    SIP_METHOD method = MessageGetMethod(message);
    
    if (matched == NULL) {    
        int account = FindMessageDestAccount(message);
        struct UserAgent *ua = AddUa(account);
        struct Dialog *d = AddDialog(NULL, ua);

        if (method == SIP_METHOD_INVITE) {
            DialogNewTransaction(d, message, TRANSACTION_TYPE_SERVER_INVITE);
        } else { 
            struct Transaction *t = DialogNewTransaction(d, message, TRANSACTION_TYPE_SERVER_NON_INVITE);
            Response(t,TRANSACTION_SEND_OK);
        }
    } else {
        DialogReceiveBye(matched, message);
    }
}

void HandleCancelEvent(struct Dialog *dialog, int event, MESSAGE *message)
{
    NotifyCm(CALL_PEER_CANCELED, DialogGetUserAgent(dialog));
}

struct TransactionEventAction TransactionEventActions[] = {
    {SIP_METHOD_REGISTER, TRANSACTION_EVENT_OK, HandleRegisterEvent},
    {SIP_METHOD_REGISTER, TRANSACTION_EVENT_UNAUTHORIZED, HandleRegisterEvent},
    {SIP_METHOD_INVITE, TRANSACTION_EVENT_OK, HandleInviteEvent},
    {SIP_METHOD_INVITE, TRANSACTION_EVENT_RINGING, HandleInviteEvent},
    {SIP_METHOD_INVITE, TRANSACTION_EVENT_NEW, HandleNewTransactionEvent},
    {SIP_METHOD_BYE,    TRANSACTION_EVENT_NEW, HandleNewTransactionEvent},
    {SIP_METHOD_CANCEL, TRANSACTION_EVENT_NEW, HandleNewTransactionEvent},
    {SIP_METHOD_CANCEL, TRANSACTION_EVENT_CANCELED, HandleCancelEvent},
    {-1, -1, 0},
};

void OnTransactionEventImpl(struct Dialog *dialog,  int event, MESSAGE *message)
{
    struct Transaction *t;
    SIP_METHOD method;
    SIP_METHOD requestMethod;
    struct TransactionEventAction *action;

    method = MessageGetMethod(message);
    if (dialog == NULL) {
        requestMethod = method;
    } else {
        t = dialog->transaction;
        if (method != SIP_METHOD_CANCEL) {
            requestMethod = MessageGetMethod(GetTransactionRequest(t));
        } else {
            requestMethod = SIP_METHOD_CANCEL;
        }
    }

    action = TransactionEventActions;    
    for (; action->requestMethod != -1; action ++)
    {
        if (requestMethod == action->requestMethod && event == action->event) {
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
            NotifyCm(CALL_INCOMING, DialogGetUserAgent(dialog));
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
    MESSAGE *bye = BuildByeMessage(dialog);
    DialogNewTransaction(dialog, bye, TRANSACTION_TYPE_CLIENT_NON_INVITE);
    dialog->state = DIALOG_STATE_TERMINATED;
}
void (*DialogTerminate)(struct Dialog *dialog) = DialogTerminateImpl;

void DialogRingingImpl(struct Dialog *dialog)
{
    Response(dialog->transaction, TRANSACTION_SEND_RINGING);
}
void (*DialogRinging)(struct Dialog *dialog) = DialogRingingImpl;


struct Message *DialogBuildInvite(struct Dialog *dialog, char *to)
{
    DialogSetRemoteUri(dialog, to);
    struct Message *invite = BuildInviteMessage(dialog, to);    

    return invite;
}

void DialogInvite(struct Dialog *dialog)
{
    struct Message *invite = DialogBuildInvite(dialog, (char *)"88002");
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
