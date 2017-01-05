#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "Header.h"
#include "Headers.h"
#include "Parser.h"
#include "Messages.h"
#include "ViaHeader.h"
#include "ContactHeader.h"
#include "CallIdHeader.h"
#include "CSeqHeader.h"
#include "ContentLengthHeader.h"
#include "StatusLine.h"
#include "ExpiresHeader.h"
#include "Parameter.h"
#include "RequestLine.h"
#include "Sdp.h"
#include "URI.h"

struct MessageDestation {
    char addr[64];
    int port;
};
struct Message {
    struct Transaction *transaction;
    union {
        struct RequestLine *request;
        struct StatusLine *status;
    } rr;
    enum MESSAGE_TYPE type;
    struct MessageDestation dest;
    struct Headers *headers;
};


void SetMessageType(MESSAGE *message, enum MESSAGE_TYPE type)
{
    assert (message != NULL);
    message->type = type;
}

enum MESSAGE_TYPE MessageGetType(MESSAGE *message)
{
    assert(message != NULL);
    return message->type;
}

enum MESSAGE_TYPE ParseMessageType(char *line)
{
    if (strncmp(line, SIP_VERSION, strlen(SIP_VERSION)) == 0){
        return MESSAGE_TYPE_RESPONSE;
    } else {
        return MESSAGE_TYPE_REQUEST;
    }
}

SIP_METHOD MessageGetMethod(MESSAGE *message)
{
    struct RequestLine *rl = MessageGetRequestLine(message);
    return RequestLineGetMethod(rl);
}

void MessageParseRequestLine(char *string, MESSAGE *message)
{
    struct RequestLine *requestLine = CreateEmptyRequestLine();

    ParseRequestLine(string, requestLine); 
    message->rr.request = requestLine;
}

void MessageParseStatusLine(char *string, MESSAGE *message)
{
    struct StatusLine *statusLine = CreateStatusLine(0, "");

    ParseStatusLine(string, statusLine);
    message->rr.status = statusLine;

}

void MessageAddHeader(MESSAGE *message, struct Header *header)
{
    RawHeadersAddHeader(message->headers, header);
}

struct Header *MessageGetHeader(const char *name, MESSAGE *message)
{
    assert (name != NULL);
    assert (message != NULL);
    return RawHeadersGetHeader(name, message->headers);
}

void ParseHeader(char *headerString, MESSAGE *message)
{
    RawParseHeader(headerString, message->headers);
}

int ParseHeaders(char *localString, MESSAGE *message)
{
    char *save_ptr = NULL;
    char *line = NULL;
    
    if (strlen(localString) == 0) {
        return -1;
    }

    line = strtok_r(localString, CRLF, &save_ptr);
    
    if (ParseMessageType(line) == MESSAGE_TYPE_REQUEST) {
        message->type = MESSAGE_TYPE_REQUEST;
        MessageParseRequestLine(line, message);
    } else {
        message->type = MESSAGE_TYPE_RESPONSE;
        MessageParseStatusLine(line, message);
    }
    
    line = strtok_r(NULL, CRLF, &save_ptr);
    while(line) {
        ParseHeader(line, message);
        line = strtok_r(NULL, CRLF, &save_ptr);
    }
    
    return 0;
}

int ParseContent(char *content, unsigned int length)
{
    ParseSdp(content, length);
    return 0;
}

int ParseMessage(const char *string, MESSAGE *message)
{    
    int error = 0;
    char *content;
    char localString[MAX_MESSAGE_LENGTH] = {0};
    
    assert(string != NULL);
    assert(message != NULL);

    strncpy(localString, string, MAX_MESSAGE_LENGTH - 1);
    content = strstr(localString, CRLFCRLF);    
    if (content != NULL) {
        *content = 0;
        content += strlen(CRLFCRLF);
    }
    
    if ((error = ParseHeaders(localString, message)) < 0)
        return error;

    if (content != NULL && MessageGetContentLength(message) != 0)
        ParseContent(content, MessageGetContentLength(message));
    
    return 0;
}

struct RequestLine *MessageGetRequestLine(MESSAGE *message)
{
    assert(message != NULL);
    return message->rr.request;
}

void SetMessageRequestLine(MESSAGE *message, struct RequestLine *rl)
{
    message->rr.request = rl;
}

struct StatusLine *MessageGetStatusLine(MESSAGE *message)
{
    assert(message != NULL);
    return message->rr.status;
}

void SetMessageStatusLine(MESSAGE *message, struct StatusLine *status)
{
    assert (message != NULL);
    assert (status != NULL);
    
    message->rr.status = status;
}

char *MessageGetCallId(MESSAGE *message)
{
    struct CallIdHeader *id = (struct CallIdHeader *) MessageGetHeader(HEADER_NAME_CALLID, message);
    return CallIdHeaderGetId(id);
}

void MessageSetCallId(MESSAGE *message, char *idString)
{
    struct CallIdHeader *id = (struct CallIdHeader *) MessageGetHeader(HEADER_NAME_CALLID, message);
    CallIdHeaderSetID(id, idString);
}

char *MessageGetFromTag(MESSAGE *message)
{
    CONTACT_HEADER *from = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_FROM, message);
    return ContactHeaderGetParameter(from, HEADER_PARAMETER_NAME_TAG);
}

void MessageSetFromTag(struct Message *message, char *tag)
{
    CONTACT_HEADER *from = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_FROM, message);
    ContactHeaderSetParameter(from, HEADER_PARAMETER_NAME_TAG, tag);    
}

char *MessageGetToTag(MESSAGE *message)
{
    char *tag;
    assert(message != NULL);

    CONTACT_HEADER *to = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_TO, message);
    tag = ContactHeaderGetParameter(to, HEADER_PARAMETER_NAME_TAG);

    if (tag == NULL) return "";
    return tag;
}

void MessageSetToTag(MESSAGE *message, char *tag)
{
    CONTACT_HEADER *to = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_TO, message);
    return ContactHeaderSetParameter(to, HEADER_PARAMETER_NAME_TAG, tag);
}

int MessageGetExpires(MESSAGE *message)
{
    struct ExpiresHeader *e = (struct ExpiresHeader *)MessageGetHeader(HEADER_NAME_EXPIRES, message); 
    return ExpiresHeaderGetExpires(e);
}

unsigned int MessageGetCSeqNumber(MESSAGE *message)
{
    assert (message != NULL);
    struct CSeqHeader *c = (struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, message);
   
    return CSeqHeaderGetSeq(c);
}

void MessageSetCSeqNumber(MESSAGE *message, int cseq)
{
    assert (message != NULL);
    struct CSeqHeader *c = (struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, message);
   
    CSeqHeaderSetSeq(c, cseq);
}

char *MessageGetViaBranch(MESSAGE *message)
{
    assert (message != NULL);
    return ViaHeaderGetParameter((VIA_HEADER *)MessageGetHeader(HEADER_NAME_VIA, message), 
                                 VIA_BRANCH_PARAMETER_NAME);
}

void MessageSetViaBranch(MESSAGE *message, char *branch)
{
    assert (message != NULL);
    assert (branch != NULL);
    
    ViaHeaderSetParameter((VIA_HEADER *)MessageGetHeader(HEADER_NAME_VIA, message),
                          VIA_BRANCH_PARAMETER_NAME,
                          branch);
}

char *MessageGetCSeqMethod(MESSAGE *message)
{
    assert (message != NULL);
    return CSeqHeaderGetMethod((struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, message));
}

void MessageAddViaParameter(MESSAGE *message, char *name, char *value)
{
    VIA_HEADER *via = (VIA_HEADER *) MessageGetHeader(HEADER_NAME_VIA, message);
    struct Parameters *p = ViaHeaderGetParameters(via);

    AddParameter(p, name, value);
}

void MessageSetCSeqMethod (MESSAGE *message, char *method)
{
    struct CSeqHeader *c = (struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, message);

    CSeqHeaderSetMethod(c, method);
}

void MessageSetContentLength(MESSAGE *message, int length)
{
    struct ContentLengthHeader *c = 
        (struct ContentLengthHeader *)MessageGetHeader(HEADER_NAME_CONTENT_LENGTH, message);
    
    ContentLengthHeaderSetLength(c, length);
}

unsigned int MessageGetContentLength(MESSAGE *message)
{    
    struct ContentLengthHeader *c = 
        (struct ContentLengthHeader *)MessageGetHeader(HEADER_NAME_CONTENT_LENGTH, message);

    if (c == NULL) 
        return 0;
    else
        return ContentLengthHeaderGetLength(c);
}

BOOL MessageViaHeaderBranchMatched(MESSAGE *m, MESSAGE *mm)
{
    return ViaHeaderBranchMatched((VIA_HEADER *)MessageGetHeader(HEADER_NAME_VIA, m),
                                  (VIA_HEADER *)MessageGetHeader(HEADER_NAME_VIA, mm));
    
}

BOOL MessageCSeqHeaderMethodMatched(MESSAGE *m, MESSAGE *mm)
{
    return  CSeqHeaderMethodMatched((struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, m),
                                    (struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, mm));    

}

BOOL MessageViaHeaderSendbyMatched(MESSAGE *m, MESSAGE *mm)
{
    return ViaHeaderSendbyMatched((VIA_HEADER *)MessageGetHeader(HEADER_NAME_VIA, m),
                                  (VIA_HEADER *)MessageGetHeader(HEADER_NAME_VIA, mm));
}

struct Transaction *MessageBelongTo(MESSAGE *message)
{
    return message->transaction;
}

void MessageSetOwner(MESSAGE *message, struct Transaction *t)
{
    message->transaction = t;
}

void Message2String(char *result, MESSAGE *message)
{
    char *p = result;

    assert(message != NULL);
    assert(result != NULL);

    if (message->type == MESSAGE_TYPE_REQUEST) {
        p = RequestLine2String(p, MessageGetRequestLine(message));
    } else {
        p = StatusLine2String(p, MessageGetStatusLine(message));
    } 

    *p++ = '\r';
    *p++ = '\n';
    
    p = RawHeaders2String(p, message->headers);
    
    *p++ = '\r';
    *p++ = '\n';

    return ;
}

char *GetMessageAddr(MESSAGE *message)
{
    return message->dest.addr;
}

void SetMessageAddr(MESSAGE *m, char *addr)
{
    strcpy(m->dest.addr, addr);
}

int  GetMessagePort(MESSAGE *m)
{
    return m->dest.port;
}

void SetMessagePort(MESSAGE *m, int port)
{
    m->dest.port = port;
}

void GetFromUser(struct Message *invite, char **from)
{
    if (invite != NULL) {
        struct ContactHeader *header = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_FROM, invite);
        struct URI *uri = ContactHeaderGetUri(header);
        *from = UriGetUser(uri);        
    }
}

void GetToUser(struct Message *invite, char **to)
{
    if (invite != NULL) {
        struct ContactHeader *header = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_TO, invite);
        struct URI *uri = ContactHeaderGetUri(header);
        *to = UriGetUser(uri);
    }
}

//Only for debug, no unit test for this funciton.
void MessageDump(MESSAGE *message)
{
    char messageString[MAX_MESSAGE_LENGTH] = {0};

    printf("\n");
    printf("%s %d\n", GetMessageAddr(message), GetMessagePort(message));
    Message2String(messageString, message);
    printf("%s\n",messageString);
}

MESSAGE *CreateMessage () 
{ 
    MESSAGE *message = calloc(1,sizeof (struct Message));
    message->type = MESSAGE_TYPE_NONE;
    message->headers = CreateHeaders();
    return message;
}

void DestroyMessage (MESSAGE **message) 
{  
    assert(message != NULL);

    if ((*message) == ((void *)0)) return;

    if ((*message)->type == MESSAGE_TYPE_REQUEST) {
        DestroyRequestLine((*message)->rr.request);
    } else {
        DestroyStatusLine((*message)->rr.status);
    }

    RawDestroyHeaders((*message)->headers);
    free(*message);
    *message = NULL;
}
