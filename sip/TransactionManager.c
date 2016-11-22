#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "Bool.h"
#include "ViaHeader.h"
#include "CSeqHeader.h"
#include "Header.h"
#include "StatusLine.h"
#include "TransactionManager.h"
#include "Transaction.h"
#include "TransactionId.h"
#include "Messages.h"
#include "MessageBuilder.h"
#include "DialogId.h"
#include "Dialog.h"
#include "RequestLine.h"

struct TransactionManager {
    t_list *transactions;
};

struct TransactionManager TransactionManager;

int CountTransaction()
{
    return get_list_len(TransactionManager.transactions);
}

struct Transaction *GetTransactionByPosition(int position)
{
    return (struct Transaction *)get_data_at(TransactionManager.transactions, position);
}

void RemoveTransactionByPosition(int position)
{
    del_node_at(&TransactionManager.transactions, position);
}

BOOL TransactionMatched(struct Transaction *t, char *branch, char *seqMethod)
{
    MESSAGE *request = GetTransactionRequest(t);

    return ViaHeaderBranchMatchedByString((VIA_HEADER *)MessageGetHeader(HEADER_NAME_VIA, request), branch) 
        && CSeqMethodMatchedByName((struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, request), seqMethod);
}

BOOL TransactionMatchedById(void *t, void *id)
{
    struct Transaction *tt = t;
    struct TransactionId *tid = id;
    return TransactionMatched((void *)tt, TransactionIdGetBranch(tid), TransactionIdGetMethod(tid));
}

BOOL TransactionMatchedByAddr(void *t1, void *t2)
{
    return t1 == t2;
}

void _RemoveTransaction(void *id, BOOL (*matched)(void *t1, void *t2))
{    
    int i = 0;

    for(; i < CountTransaction(); i++) {
        struct Transaction *tt = GetTransactionByPosition(i);
        if (matched(tt, id)) {
            RemoveTransactionByPosition(i);
            DestroyTransaction(&tt);
            break;
        }
    }
}

void RemoveTransactionById(struct TransactionId *id)
{
    _RemoveTransaction(id, TransactionMatchedById);
}

void RemoveTransaction(struct Transaction *t)
{
    _RemoveTransaction(t, TransactionMatchedByAddr);
}

struct Transaction *GetTransaction(char *branch, char *seqMethod)
{
    int i = 0;
    int length = CountTransaction();

    assert(branch != NULL && seqMethod != NULL);
    
    for (; i < length; i ++) {
        struct Transaction *t = GetTransactionByPosition(i);
        if (TransactionMatched(t, branch, seqMethod))
            return t;
    }

    return NULL;
}

struct Transaction *MatchRequest(MESSAGE *message)
{
    char *branch = MessageGetViaBranch(message);
    
    SIP_METHOD method = MessageGetMethod(message);
    if (method == SIP_METHOD_CANCEL) {
        method = SIP_METHOD_INVITE;
    }

    char *methodName = MethodMap2String(method);
    return GetTransaction(branch, methodName);
}

struct Transaction *MatchResponse(MESSAGE *message)
{
    char *branch = MessageGetViaBranch(message);
    char *method = MessageGetCSeqMethod(message);
    return GetTransaction(branch, method);
}

struct StatusCodeEventMap {
    int statusCode;
    enum TransactionEvent event;
} StatusCodeEventMap[] = {
    {200, TRANSACTION_EVENT_OK},
    {100, TRANSACTION_EVENT_TRYING},
    {180, TRANSACTION_EVENT_RINGING},
    {401, TRANSACTION_EVENT_UNAUTHORIZED},
    {0, 0},
};

enum TransactionEvent MapStatusCodeToEvent(int statusCode)
{
    for (int i = 0; StatusCodeEventMap[i].statusCode != 0; i++) {
        if (StatusCodeEventMap[i].statusCode == statusCode) {
            return StatusCodeEventMap[i].event;
        }
    }

    return -1;
}

BOOL TmHandleReponseMessage(MESSAGE *message)
{
    struct StatusLine *status = NULL;
    int statusCode = 0;
    struct Transaction *t = NULL;

    status = MessageGetStatusLine(message);
    statusCode = StatusLineGetStatusCode(status);
    t = MatchResponse(message);
    
    if (t) {
        AddResponse(t, message);
        RunFsm(t, MapStatusCodeToEvent(statusCode));
        return TRUE;
    }

    return FALSE;
}

BOOL TmHandleRequestMessage(MESSAGE *message)
{
    struct Transaction *t = MatchRequest(message);
    SIP_METHOD method = MessageGetMethod(message);
    
    if (!t) {
            OnTransactionEvent(NULL, TRANSACTION_EVENT_NEW, message);
    } else {
        if (method == SIP_METHOD_INVITE) {
            RunFsm(t, TRANSACTION_EVENT_INVITE);
            DestroyMessage(&message);
        } else if (method == SIP_METHOD_CANCEL) {
            struct Dialog *d = (struct Dialog *)GetTransactionUser(t);
            struct Message *rt = BuildResponse(d, message, 487);

            OnTransactionEvent(d, TRANSACTION_EVENT_CANCELED, message);            
            ResponseWith(t, rt, TRANSACTION_SEND_REQUEST_TERMINATED);

            OnTransactionEvent(NULL, TRANSACTION_EVENT_NEW, message);            
        }        
    }

    return TRUE;
}

BOOL SipMessageInput(struct Message *message)
{
    BOOL garbage;

    if (MessageGetType(message) == MESSAGE_TYPE_RESPONSE) {
        garbage = !TmHandleReponseMessage(message);
    } else if (MessageGetType(message) == MESSAGE_TYPE_REQUEST){
        garbage = !TmHandleRequestMessage(message);
    }
    
    if (garbage) {
        DestroyMessage(&message);
        return FALSE;
    }

    return TRUE;

}

BOOL SipMessageCallback(char *string, char *ip, int port)
{
    MESSAGE *message = CreateMessage();

    SetMessageAddr(message, ip);
    SetMessagePort(message, port);
    
    if (ParseMessage(string, message) < 0) {
        return FALSE;
    }

    return SipMessageInput(message);
}

void DestroyTransactions(struct TransactionManager *manager)
{
    int i = 0;
    int length = CountTransaction();

    for ( ; i < length; i++) {
        struct Transaction *t = GetTransactionByPosition(i);
        DestroyTransaction((struct Transaction **)&t);
    }

    destroy_list(&manager->transactions, NULL);
}

void ClearTransactionManager()
{    
    DestroyTransactions(&TransactionManager);
}

struct TransactionManager TransactionManager;

void _AddTransaction(struct Transaction *t)
{
    if (t == NULL)  return;
    
    SetTransactionObserver(t, &TransactionManager);
    put_in_list(&TransactionManager.transactions, t);
}

BOOL ValidatedNonInviteMethod(MESSAGE *message)
{
    struct RequestLine *rl = MessageGetRequestLine(message);
    return  RequestLineGetMethod(rl) != SIP_METHOD_INVITE 
        &&  RequestLineGetMethod(rl) != SIP_METHOD_ACK;
}

struct Transaction *AddTransactionImpl(MESSAGE *message, struct TransactionUser *user, int type)
{
    if (type == TRANSACTION_TYPE_SERVER_NON_INVITE) {
        if (!ValidatedNonInviteMethod(message)) return NULL;
    }

    struct Transaction *t = CreateTransaction(message, user, type);
    _AddTransaction(t);
    RunFsm(t, TRANSACTION_EVENT_INIT);
    
    return t;

}

struct Transaction *(*AddTransaction)(MESSAGE *message, struct TransactionUser *user, int type) = AddTransactionImpl;

void DumpTransactionManager()
{
    int i = 0;
    int length = CountTransaction();

    printf("\n");
    for ( ; i < length; i++) {
        struct Transaction *t = GetTransactionByPosition(i);
        DumpTransaction(t);
    }
}
