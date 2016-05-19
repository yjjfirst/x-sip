#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "Bool.h"
#include "ViaHeader.h"
#include "CSeqHeader.h"
#include "Header.h"
#include "StatusLine.h"
#include "TransactionManager.h"
#include "TransactionNotifiers.h"
#include "Transaction.h"
#include "TransactionId.h"
#include "Messages.h"
#include "DialogId.h"
#include "Dialog.h"
#include "RequestLine.h"

struct TransactionManager {
    struct TransactionManagerObserver observer;
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
    struct Message *request = TransactionGetRequest(t);
    
    return ViaHeaderBranchMatchedByString((struct ViaHeader *)MessageGetHeader(HEADER_NAME_VIA, request), branch) 
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
            DestoryTransaction(&tt);
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

struct Transaction *MatchTransaction(struct Message *message)
{
    char *branch = MessageGetViaBranch(message);
    char *method = MessageGetCSeqMethod(message);
    
    return GetTransaction(branch, method);
}

BOOL TmHandleReponseMessage(struct Message *message)
{
    struct StatusLine *status = NULL;
    int statusCode = 0;
    struct Transaction *t = NULL;

    status = MessageGetStatusLine(message);
    statusCode = StatusLineGetStatusCode(status);
        
    if ( (t = MatchTransaction(message)) != NULL) {
        TransactionAddResponse(t, message);
        if (statusCode == 200) {
            RunFsm(t, TRANSACTION_EVENT_200OK_RECEIVED);
        }
        else if (statusCode == 100) {
            RunFsm(t, TRANSACTION_EVENT_100TRYING_RECEIVED);
        }
        
        return TRUE;
    }

    return FALSE;
}

BOOL TmHandleRequestMessage(struct Message *message)
{
    struct Transaction *t = NULL;

    if ( (t = MatchTransaction(message)) == NULL) {
        AddServerInviteTransaction(message, NULL);
    } else {
        RunFsm(t, TRANSACTION_EVENT_INVITE_RECEIVED);
        DestoryMessage(&message);
    }

    return TRUE;
}

BOOL MessageReceived(char *string)
{
    struct Message *message = CreateMessage();
    BOOL garbage;

    if (ParseMessage(string, message) < 0) {
        return FALSE;
    }
    
    if (MessageGetType(message) == MESSAGE_TYPE_RESPONSE) {
        garbage = !TmHandleReponseMessage(message);
    } else if (MessageGetType(message) == MESSAGE_TYPE_REQUEST){
        garbage = !TmHandleRequestMessage(message);
    }
    
    if (garbage) {
        DestoryMessage(&message);
        return FALSE;
    }
    
    return TRUE;
}

void DestoryTransactions(struct TransactionManager *manager)
{
    int i = 0;
    int length = CountTransaction();

    for ( ; i < length; i++) {
        struct Transaction *t = GetTransactionByPosition(i);
        DestoryTransaction((struct Transaction **)&t);
    }

    destroy_list(&manager->transactions, NULL);
}

void EmptyTransactionManager()
{
    DestoryTransactions(&TransactionManager);
}

struct TransactionManager TransactionManager = {
    {RemoveTransaction},
};

void AddTransaction2Manager(struct Transaction *t)
{
    if (t != NULL) {
        TransactionSetObserver(t, &TransactionManager.observer);
        put_in_list(&TransactionManager.transactions, t);
    }
}

BOOL ValidatedNonInviteMethod(struct Message *message)
{
    struct RequestLine *rl = MessageGetRequestLine(message);
    return  RequestLineGetMethod(rl) != SIP_METHOD_INVITE 
        &&  RequestLineGetMethod(rl) != SIP_METHOD_ACK;
}

struct Transaction *AddClientNonInviteTransaction(struct Message *message, struct TransactionUserObserver *user)
{
    struct Transaction *t = CreateClientNonInviteTransaction(message, user);

    AddTransaction2Manager(t);
    return t;
}

struct Transaction *AddClientInviteTransaction(struct Message *message, struct TransactionUserObserver *user)
{
    struct Transaction *t = CreateClientInviteTransaction(message, user);

    AddTransaction2Manager(t);
    return t;
}


struct Transaction *AddServerInviteTransaction(struct Message *message, struct TransactionUserObserver *user)
{
    struct Transaction *t = CreateServerInviteTransaction(message, user);

    AddTransaction2Manager(t);
    return t;
}

struct Transaction *AddServerNonInviteTransaction(struct Message *message, struct TransactionUserObserver *user)
{
    struct Transaction *t = NULL;

    assert(message != NULL);
    if (!ValidatedNonInviteMethod(message)) return NULL;

    t = CreateServerNonInviteTransaction(message, user);
    AddTransaction2Manager(t);

    return t;
}
