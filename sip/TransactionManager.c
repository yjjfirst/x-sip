#include <stdlib.h>
#include <stdio.h>

#include "Bool.h"
#include "ViaHeader.h"
#include "CSeqHeader.h"
#include "Header.h"
#include "StatusLine.h"
#include "TransactionManager.h"
#include "Transaction.h"
#include "Messages.h"

static struct TransactionManager *SingletonTransactionManager;

int CountTransaction()
{
    return get_list_len(GetTransactionManager()->transactions);
}

struct Transaction *CreateTransactionExt(struct Message *message)
{
    struct Transaction *t = CreateTransaction(message);
    put_in_list(&SingletonTransactionManager->transactions, t);
    
    return t;
}

struct TransactionManager *GetTransactionManager()
{
    if (SingletonTransactionManager == NULL) {
        SingletonTransactionManager = calloc(1, sizeof(struct TransactionManager));
        SingletonTransactionManager->CreateTransaction = CreateTransactionExt;
        SingletonTransactionManager->CountTransaction = CountTransaction;
    }

    return SingletonTransactionManager;
}

BOOL MatchResponse(struct Message *request, struct Message *response)
{
    return ViaBranchMatched((struct ViaHeader *)MessageGetHeader(HEADER_NAME_VIA, request),
                     (struct ViaHeader *)MessageGetHeader(HEADER_NAME_VIA, response)) 
        && CSeqHeaderMethodMatched((struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, request),
                                   (struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, response));
}

struct Transaction *MatchTransaction(struct Message *message)
{
    int i = 0;
    int length = CountTransaction();
    struct Transaction *t = NULL;
    
    for (; i < length; i++) {
        struct Transaction *tt = get_data_at(SingletonTransactionManager->transactions, i);
        if (MatchResponse(TransactionGetRequest(tt), message))
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

    ParseMessage(string, message);
    status = MessageGetStatus(message);
    statusCode = StatusLineGetStatusCode(status);

    if ( (t = MatchTransaction(message)) != NULL) {
        printf("asdf");
        if (statusCode == 200) {
            RunFSM(t, TRANSACTION_EVENT_200OK);
        }
        else if (statusCode == 100) {
            RunFSM(t, TRANSACTION_EVENT_100TRYING);
        }

        TransactionAddResponse(t, message);
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
        struct Transaction *t = get_data_at(manager->transactions, i);
        DestoryTransaction((struct Transaction **)&t);
    }
    
    destroy_list(&manager->transactions, NULL);
}

void DestoryTransactionManager(struct TransactionManager **manager)
{
    if (*manager != NULL) {
        DestoryTransactions(*manager);
        free(*manager);
        SingletonTransactionManager = NULL;
        *manager = NULL;
    }
}
