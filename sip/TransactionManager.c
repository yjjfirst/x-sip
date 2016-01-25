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
#include "Messages.h"
#include "DialogId.h"
#include "Dialog.h"

struct TransactionManager {
    struct TransactionNotifiers *notifiers;
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

void RemoveTransaction(struct Transaction *t)
{
    int i = 0;

    for(; i < CountTransaction(); i++) {
        struct Transaction *tt = GetTransactionByPosition(i);
        if (tt == t) {
            RemoveTransactionByPosition(i);
            DestoryTransaction(&tt);
            break;
        }
    }
}

BOOL MatchTransactionByString(struct Transaction *t, char *branch, char *seqMethod)
{
    struct Message *request = TransactionGetRequest(t);
    
    return ViaHeaderBranchMatchedByString((struct ViaHeader *)MessageGetHeader(HEADER_NAME_VIA, request), branch) 
        && CSeqMethodMatchedByName((struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, request), seqMethod);
}

struct Transaction *GetTransaction(char *branch, char *seqMethod)
{
    int i = 0;
    int length = CountTransaction();
    assert(branch != NULL && seqMethod != NULL);
    
    for (; i < length; i ++) {
        struct Transaction *t = GetTransactionByPosition(i);
        if (MatchTransactionByString(t, branch, seqMethod))
            return t;
    }

    return NULL;
}

struct Transaction *MatchTransaction(struct Message *message)
{
    int i = 0;
    int length = CountTransaction();
    struct Transaction *t = NULL;
    
    assert (message != NULL);
    for (; i < length; i++) {
        struct Transaction *tt = GetTransactionByPosition(i);
        struct Message *request = TransactionGetRequest(tt);
        if (RequestResponseMatched(request, message))
            t = tt;
    }
    
    return t;
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
        AddServerTransaction(message,NULL);
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

struct TransactionNotifiers Notifiers = {
    .die = RemoveTransaction,
};

struct TransactionManager TransactionManager = {
    .notifiers = &Notifiers,
};

struct Transaction *AddClientTransaction(struct Message *message, struct TransactionOwner *owner)
{
    struct Transaction *t = CreateClientTransaction(message, owner);

    if (t != NULL) {
        TransactionSetNotifiers(t, TransactionManager.notifiers);
        put_in_list(&TransactionManager.transactions, t);
    }

    return t;
}

struct Transaction *AddServerTransaction(struct Message *message, struct TransactionOwner *owner)
{
    struct Transaction *t = CreateServerTransaction(message, owner);

    if (t != NULL) {
        TransactionSetNotifiers(t, TransactionManager.notifiers);
        put_in_list(&TransactionManager.transactions, t);
    }

    return t;
}
