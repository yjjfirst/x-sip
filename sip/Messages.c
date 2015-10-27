#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Header.h"
#include "Parser.h"
#include "Messages.h"
#include "ViaHeader.h"
#include "MaxForwardsHeader.h"
#include "ContactHeader.h"
#include "CallIdHeader.h"
#include "CSeqHeader.h"
#include "ContentLengthHeader.h"
#include "StatusLine.h"
#include "ExpiresHeader.h"
#include "utils/list/include/list.h"

#define MAX_MESSAGE_LENGTH 2048
#define SIP_VERSION "SIP/2.0"

enum MESSAGE_TYPE {
    MESSAGE_TYPE_REQUEST,
    MESSAGE_TYPE_RESPONSE
};

struct Message {
    union {
        struct RequestLine *request;
        struct StatusLine *status;
    } rr;
    enum MESSAGE_TYPE type;
    t_list *headers;
};

struct HeaderOperation {
    char name[HEADER_NAME_MAX_LENGTH];
    struct Header * (*headerParser)(char *headerString);
    void (*headerDestroyer)(struct Header *header);
    char *(*toString)(char *result, struct Header *header);
};

struct HeaderOperation HeaderOperations[] = {
    {HEADER_NAME_VIA, ParseViaHeader, DestoryViaHeader, ViaHeader2String},
    {HEADER_NAME_MAX_FORWARDS, ParseMaxForwardsHeader, DestoryMaxForwardsHeader,MaxForwards2String},
    {HEADER_NAME_TO, ParseContactHeader, DestoryContactHeader,ContactHeader2String},
    {HEADER_NAME_FROM, ParseContactHeader, DestoryContactHeader, ContactHeader2String},
    {HEADER_NAME_CONTACT, ParseContactHeader, DestoryContactHeader, ContactHeader2String},
    {HEADER_NAME_CALLID, ParseCallIdHeader, DestoryCallIdHeader, CallIdHeader2String},
    {HEADER_NAME_CSEQ, ParseCSeqHeader, DestoryCSeqHeader, CSeq2String},
    {HEADER_NAME_CONTENT_LENGTH, ParseContentLength, DestoryContentLengthHeader, ContentLengthHeader2String},
    {HEADER_NAME_EXPIRES, ParseExpiresHeader, DestoryExpiresHeader, ExpiresHeader2String},
};

int CountHeaderOperations()
{
    return sizeof(HeaderOperations)/sizeof(struct HeaderOperation);    
}

void ExtractHeaderName(char *header, char *name)
{
    char *end = strchr (header, COLON);

    end --;
    while (*end == SPACE) end --;
    while (*header == SPACE) header ++;

    strncpy(name, header, end - header + 1);
}

enum MESSAGE_TYPE ParseMessageType(char *line)
{
    if (strncmp(line, SIP_VERSION, strlen(SIP_VERSION)) == 0){
        return MESSAGE_TYPE_RESPONSE;
    } else {
        return MESSAGE_TYPE_REQUEST;
    }
}

void MessageParseRequestLine(char *string, struct Message *message)
{
    struct RequestLine *requestLine = CreateEmptyRequestLine();

    ParseRequestLine(string, requestLine); 
    message->rr.request = requestLine;
}

void MessageParseStatusLine(char *string, struct Message *message)
{
    struct StatusLine *statusLine = CreateStatusLine();

    ParseStatusLine(string, statusLine);
    message->rr.status = statusLine;

}

void MessageAddHeader(struct Message *message, struct Header *header)
{
    put_in_list(&message->headers, header);
}

struct Header *MessageGetHeader(const char *name, struct Message *message)
{
    int length = get_list_len(message->headers);
    int i = 0;
    struct Header *header = NULL;

    for (i = 0 ; i < length; i++) {
        struct Header *h = (struct Header *) get_data_at(message->headers, i);
        if (strcmp (name, HeaderGetName(h)) == 0) {
            header = h;
            break;
        }
    }

    return header;
}

void ParseHeader(char *headerString, struct Message *message)
{
    char name[32] = {0};
    int i = 0;
    
    ExtractHeaderName(headerString, name);

    for ( ; i < CountHeaderOperations(); i++) {
        if (strcmp (HeaderOperations[i].name , name) == 0)
            put_in_list(&message->headers, (void*) HeaderOperations[i].headerParser(headerString));
    }
}

int ParseMessage(char *string, struct Message *message)
{
    char localString[MAX_MESSAGE_LENGTH] = {0};
    char *save_ptr = NULL;

    strncpy(localString, string, MAX_MESSAGE_LENGTH - 1);
    char *line = strtok_r(localString, "\r\n", &save_ptr);
    
    if (ParseMessageType(line) == MESSAGE_TYPE_REQUEST) {
        message->type = MESSAGE_TYPE_REQUEST;
        MessageParseRequestLine(line, message);
    }
    else {
        message->type = MESSAGE_TYPE_RESPONSE;
        MessageParseStatusLine(line, message);
    }
    
    line = strtok_r(NULL, "\r\n", &save_ptr);
    while(line) {
        ParseHeader(line, message);
        line = strtok_r(NULL, "\r\n", &save_ptr);
    }
    
    return 0;
}

struct RequestLine *MessageGetRequest(struct Message *message)
{
    return message->rr.request;
}

void MessageSetRequest(struct Message *message, struct RequestLine *rl)
{
    message->rr.request = rl;
}

struct StatusLine *MessageGetStatus(struct Message *message)
{
    return message->rr.status;
}

char *Header2String(char *result, struct Header *header)
{
    int i = 0;
    
    for ( ; i < CountHeaderOperations(); i++) {
        if (strcmp (HeaderOperations[i].name , HeaderGetName(header)) == 0)
            return HeaderOperations[i].toString(result, header);        
    }

    return result;
}

void Message2String(char *result, struct Message *message)
{
    int length = get_list_len(message->headers);
    int i = 0;
    struct Header *header = NULL;
    char *p = result;
    
    if (message->type == MESSAGE_TYPE_REQUEST) {
        p = RequestLine2String(p, MessageGetRequest(message));
    } else {
        p = StatusLine2String(p, MessageGetStatus(message));
    }

    for (i = 0 ; i < length; i++) {        
        header = (struct Header *) get_data_at(message->headers, i);
        p = Header2String(p, header);
        *p ++ = '\r';
        *p ++ = '\n';
    }

    return ;
}

struct Message *CreateMessage () 
{ 
    struct Message *message = calloc(1,sizeof (struct Message));
    put_in_list (&message->headers, "");
    return message;
}

void DestoryOneHeader(struct Header *header)
{
    int i;

    for (i = 0 ; i < CountHeaderOperations(); i++) {
        if (strcmp (HeaderOperations[i].name, header->name) == 0) {
            HeaderOperations[i].headerDestroyer(header);
        }
    }
}

void MessageDestoryHeaders(struct Message *message)
{
    int length = get_list_len(message->headers);
    int i ;
    struct Header *header = NULL;

    for (i = 0 ; i < length; i++) {        
        header = (struct Header *) get_data_at(message->headers, i);
        DestoryOneHeader(header);
    }
}

void DestoryMessage (struct Message **message) 
{ 
    if ((*message) != ((void *)0)) {
        if ((*message)->type == MESSAGE_TYPE_REQUEST) 
            DestoryRequestLine((*message)->rr.request);
        else
            DestoryStatusLine((*message)->rr.status);
        MessageDestoryHeaders(*message);
        destroy_list(&(*message)->headers, NULL);
        free(*message);
        *message = NULL;
    }
}

