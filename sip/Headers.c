#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Parser.h"
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

void RawDestoryHeaders(t_list *headers)
{
    int length = get_list_len(headers);
    int i ;
    struct Header *header = NULL;

    assert(headers != NULL);
    for (i = 0 ; i < length; i++) {        
        header = (struct Header *) get_data_at(headers, i);
        DestoryOneHeader(header);
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

void RawHeadersAddHeader(t_list *headers, struct Header *header)
{
    put_in_list(&headers, header);
}

int HeadersLength(struct Headers *headers)
{
    return get_list_len(headers->headerList);
}

struct Header *RawHeadersGetHeader(const char *name, t_list *headers)
{
    int length = 0; 
    int i = 0;
    struct Header *header = NULL;

    length = get_list_len(headers);
    for (i = 0 ; i < length; i++) {
        struct Header *h = (struct Header *) get_data_at(headers, i);
        if (strcmp (name, HeaderGetName(h)) == 0) {
            header = h;
            break;
        }
    }
    
    return header;
   
}

void ExtractHeaderName(char *header, char *name)
{
    char *end = strchr (header, COLON);

    end --;
    while (*end == SPACE) end --;
    while (*header == SPACE) header ++;

    strncpy(name, header, end - header + 1);
}

void RawParseHeader(char *string, t_list *headers)
{
    char name[32] = {0};
    int i = 0;
    
    ExtractHeaderName(string, name);

    for ( ; i < CountHeaderOps(); i++) {
        if (strcmp (HeaderOps[i].name , name) == 0)
            put_in_list(&headers, (void*) HeaderOps[i].headerParser(string));
    }

}

char *Header2String(char *result, struct Header *header)
{
    int i = 0;
    
    assert(result != NULL);
    assert(header != NULL);
    for ( ; i < CountHeaderOps(); i++) {
        if (strcmp (HeaderOps[i].name , HeaderGetName(header)) == 0)
            return HeaderOps[i].toString(result, header);        
    }

    return result;
}

char *RawHeaders2String(char *result, t_list *headers)
{
    int length = get_list_len(headers);
    int i = 0;
    struct Header *header = NULL;
    char *p = result;

    for (i = 0 ; i < length; i++) {        
        header = (struct Header *) get_data_at(headers, i);
        p = Header2String(p, header);
        *p ++ = '\r';
        *p ++ = '\n';
    }

    return p;
}
