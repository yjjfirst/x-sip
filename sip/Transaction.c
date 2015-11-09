#include <stdio.h>

#include "Bool.h"
#include "Timer.h"
#include "Transaction.h"
#include "MessageBuilder.h"
#include "Messages.h"
#include "MessageTransport.h"
#include "utils/list/include/list.h"
#include "StatusLine.h"
#include "Header.h"
#include "ViaHeader.h"
#include "CSeqHeader.h"

#define T1 500
#define T2 4000

struct Transaction {
    enum TransactionState state;
    struct Message *request;
    t_list *responses; 
};

struct Transaction  *Transaction;

enum TransactionState TransactionGetState(struct Transaction *t)
{
    return t->state;
}

BOOL MatchResponse(struct Message *request, struct Message *response)
{
    return ViaBranchMatched((struct ViaHeader *)MessageGetHeader(HEADER_NAME_VIA, request), 
                            (struct ViaHeader *)MessageGetHeader(HEADER_NAME_VIA, response))
        && CSeqHeaderMethodMatched((struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, request),
                                   (struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, response));
}

int TransactionHandleMessage(char *string)
{
    struct Message *message = CreateMessage();
    struct StatusLine *status = NULL;
    int statusCode = 0;

    ParseMessage(string, message);
    status = MessageGetStatus(message);
    statusCode = StatusLineGetStatusCode(status);
    
    if (MatchResponse(Transaction->request, message)){
        if (statusCode == 200)
            Transaction->state = TRANSACTION_STATE_COMPLETED;
        else if (statusCode == 100)
            Transaction->state = TRANSACTION_STATE_PROCEEDING;
    }

    DestoryMessage(&message);
    return 0;
}

struct Transaction *CreateTransaction(struct Message *request)
{
    struct Transaction *t;
    char s[MAX_MESSAGE_LENGTH] = {0};

    t = calloc(1, sizeof (struct Transaction));
    t->state = TRANSACTION_STATE_TRYING;
    t->request = request;

    Message2String(s, request);
    SendMessage(s);
    //AddTimer(T1);

    Transaction = t;

    return t;
}

void DestoryTransaction(struct Transaction **t)
{
    free(*t);
    *t = NULL;
}

void InitTransactionLayer()
{
    InitReceiveMessageCallback(TransactionHandleMessage);
}
