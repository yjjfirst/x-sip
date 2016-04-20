#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Header.h"
#include "Messages.h"
#include "ViaHeader.h"
#include "MaxForwardsHeader.h"
#include "ContactHeader.h"
#include "CallIdHeader.h"
#include "CSeqHeader.h"
#include "ContentLengthHeader.h"
#include "StatusLine.h"
#include "ExpiresHeader.h"
#include "Parameter.h"

#include "utils/list/include/list.h"

struct Headers {
    t_list *headerList;
};

struct HeaderOp {
    char name[HEADER_NAME_MAX_LENGTH];
    struct Header * (*headerParser)(char *headerString);
    void (*headerDestroyer)(struct Header *header);
    char *(*toString)(char *result, struct Header *header);
};

struct HeaderOp HeaderOps[] = {
    {HEADER_NAME_VIA, ParseViaHeader, DestoryViaHeader, ViaHeader2String},
    {HEADER_NAME_MAX_FORWARDS, ParseMaxForwardsHeader, DestoryMaxForwardsHeader,MaxForwards2String},
    {HEADER_NAME_TO, ParseContactHeader, DestoryContactHeader,ContactHeader2String},
    {HEADER_NAME_FROM, ParseContactHeader, DestoryContactHeader, ContactHeader2String},
    {HEADER_NAME_CONTACT, ParseContactHeader, DestoryContactHeader, ContactHeader2String},
    {HEADER_NAME_CALLID, ParseCallIdHeader, DestoryCallIdHeader, CallIdHeader2String},
    {HEADER_NAME_CSEQ, ParseCSeqHeader, DestoryCSeqHeader, CSeq2String},
    {HEADER_NAME_CONTENT_LENGTH, ParseContentLengthHeader, DestoryContentLengthHeader, ContentLengthHeader2String},
    {HEADER_NAME_EXPIRES, ParseExpiresHeader, DestoryExpiresHeader, ExpiresHeader2String},
};


struct Headers *CreateHeaders()
{
    return calloc(sizeof(struct Headers), 1);
}

int CountHeaderOps()
{
    return sizeof(HeaderOps)/sizeof(struct HeaderOp);    
}

static void DestoryOneHeader(struct Header *header)
{
    int i;

    assert(header != NULL);
    for (i = 0 ; i < CountHeaderOps(); i++) {
        if (strcmp (HeaderOps[i].name, header->name) == 0) {
            HeaderOps[i].headerDestroyer(header);
            return;
        }
    }
}

void DestoryHeaders(struct Headers **headers)
{
    int len = 0;
    int i = 0;
    struct Header *header = NULL;

    if (*headers == NULL) return; 
        
    len = get_list_len((*headers)->headerList);
    for (; i < len; i++) {
        header = (struct Header *) get_data_at((*headers)->headerList, i);
        DestoryOneHeader(header);
    }
    free(*headers);
    *headers = NULL;
}

void HeadersAddHeader(struct Headers *headers, struct Header *header)
{
    put_in_list(&headers->headerList, header);
}

int HeadersLength(struct Headers *headers)
{
    return get_list_len(headers->headerList);
}
