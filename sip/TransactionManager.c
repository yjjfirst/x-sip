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

struct TransactionManager {
    struct TransactionNotifiers *notifiers;
    t_list *transactions;
};

struct TransactionManager TransactionManager;

int CountTransaction()
{
    return get_list_len(TransactionManager.transactions);
}

struct TransactionId *ExtractTransactionIdFromMessage(struct Message *message)
{
    //ViaHeaderGetParameter(via, VIA_BRANCH_PARAMETER_NAME);
    return NULL;
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
    struct Message *m = TransactionGetRequest(t);
    
    return ViaHeaderBranchMatchedByString((struct ViaHeader *)MessageGetHeader(HEADER_NAME_VIA, m), branch) 
        && CSeqMethodMatchedByString((struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, m), seqMethod);
}

struct Transaction *GetTransactionBy(char *branch, char *seqMethod)
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
    
    for (; i < length; i++) {
        struct Transaction *tt = GetTransactionByPosition(i);
        if (RequestResponseMatched(TransactionGetRequest(tt), message))
            t = tt;
    }
    
    return t;
}

BOOL MessageReceived(char *string)
{
    struct Message *message = CreateMessage();
    struct StatusLine *status = NULL;
    int statusCode = 0;
    struct Transaction *t = NULL;

    if (ParseMessage(string, message) < 0) {
        return FALSE;
    }
    
    status = MessageGetStatusLine(message);
    statusCode = StatusLineGetStatusCode(status);

    if ( (t = MatchTransaction(message)) != NULL) {
        TransactionAddResponse(t, message);
        if (statusCode == 200) {
            RunFSM(t, TRANSACTION_EVENT_200OK);
        }
        else if (statusCode == 100) {
            RunFSM(t, TRANSACTION_EVENT_100TRYING);
        }
        
        return TRUE;
    }

    DestoryMessage(&message);
    return FALSE;
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

struct Transaction *AddTransaction(struct Message *message, struct TransactionOwner *owner)
{
    struct Transaction *t = CreateTransaction(message, owner);

    if (TransactionManager.notifiers == NULL) {
        TransactionManager.notifiers = &Notifiers;
    }

    if (t != NULL) {
        TransactionSetNotifiers(t, TransactionManager.notifiers);
        put_in_list(&TransactionManager.transactions, t);
    }

    return t;
}
