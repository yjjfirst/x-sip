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

struct Message {
    union {
        struct RequestLine *request;
        struct StatusLine *status;
    } rr;
    enum MESSAGE_TYPE type;
    struct Headers *headers;
};

void MessageSetType(struct Message *message, enum MESSAGE_TYPE type)
{
    assert (message != NULL);
    message->type = type;
}

enum MESSAGE_TYPE MessageGetType(struct Message *message)
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

void MessageParseRequestLine(char *string, struct Message *message)
{
    struct RequestLine *requestLine = CreateEmptyRequestLine();

    ParseRequestLine(string, requestLine); 
    message->rr.request = requestLine;
}

void MessageParseStatusLine(char *string, struct Message *message)
{
    struct StatusLine *statusLine = CreateStatusLine(0, "");

    ParseStatusLine(string, statusLine);
    message->rr.status = statusLine;

}

void MessageAddHeader(struct Message *message, struct Header *header)
{
    RawHeadersAddHeader(message->headers, header);
}

struct Header *MessageGetHeader(const char *name, struct Message *message)
{
    return RawHeadersGetHeader(name, message->headers);
}

void ParseHeader(char *headerString, struct Message *message)
{
    RawParseHeader(headerString, message->headers);
}

int ParseMessage(char *string, struct Message *message)
{
    char localString[MAX_MESSAGE_LENGTH] = {0};
    char *save_ptr = NULL;

    if (strlen(string) == 0) {
        return -1;
    }

    strncpy(localString, string, MAX_MESSAGE_LENGTH - 1);
    char *line = strtok_r(localString, CRLF, &save_ptr);
    
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

struct RequestLine *MessageGetRequestLine(struct Message *message)
{
    return message->rr.request;
}

void MessageSetRequestLine(struct Message *message, struct RequestLine *rl)
{
    message->rr.request = rl;
}

struct StatusLine *MessageGetStatusLine(struct Message *message)
{
    assert(message != NULL);
    return message->rr.status;
}

void MessageSetStatusLine(struct Message *message, struct StatusLine *status)
{
    assert (message != NULL);
    assert (status != NULL);
    
    message->rr.status = status;
}

char *MessageGetCallId(struct Message *message)
{
    struct CallIdHeader *id = (struct CallIdHeader *) MessageGetHeader(HEADER_NAME_CALLID, message);
    return CallIdHeaderGetId(id);
}

char *MessageGetFromTag(struct Message *message)
{
    struct ContactHeader *from = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_FROM, message);
    return ContactHeaderGetParameter(from, HEADER_PARAMETER_NAME_TAG);
}

char *MessageGetToTag(struct Message *message)
{
    struct ContactHeader *to = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_TO, message);
    return ContactHeaderGetParameter(to, HEADER_PARAMETER_NAME_TAG);
}

void MessageSetRemoteTag(struct Message *message, char *tag)
{
    struct ContactHeader *to = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_TO, message);
    return ContactHeaderSetParameter(to, HEADER_PARAMETER_NAME_TAG, tag);
}

int MessageGetExpires(struct Message *message)
{
    struct ExpiresHeader *e = (struct ExpiresHeader *)MessageGetHeader(HEADER_NAME_EXPIRES, message); 
    return ExpiresHeaderGetExpires(e);
}

unsigned int MessageGetCSeqNumber(struct Message *message)
{
    assert (message != NULL);
    struct CSeqHeader *c = (struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, message);
    
    return CSeqHeaderGetSeq(c);
}

char *MessageGetViaBranch(struct Message *message)
{
    assert (message != NULL);
    return ViaHeaderGetParameter((struct ViaHeader *)MessageGetHeader(HEADER_NAME_VIA, message), 
                                 VIA_BRANCH_PARAMETER_NAME);
}

void MessageSetViaBranch(struct Message *message, char *branch)
{
    assert (message != NULL);
    assert (branch != NULL);
    
    ViaHeaderSetParameter((struct ViaHeader *)MessageGetHeader(HEADER_NAME_VIA, message),
                          VIA_BRANCH_PARAMETER_NAME,
                          branch);
}

char *MessageGetCSeqMethod(struct Message *message)
{
    assert (message != NULL);
    return CSeqHeaderGetMethod((struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, message));
}

void MessageAddViaParameter(struct Message *message, char *name, char *value)
{
    struct ViaHeader *via = (struct ViaHeader *) MessageGetHeader(HEADER_NAME_VIA, message);
    struct Parameters *p = ViaHeaderGetParameters(via);

    AddParameter(p, name, value);
}

void MessageSetCSeqMethod (struct Message *message, char *method)
{
    struct CSeqHeader *c = (struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, message);

    CSeqHeaderSetMethod(c, method);
}

void MessageSetContentLength(struct Message *message, int length)
{
    struct ContentLengthHeader *c = (struct ContentLengthHeader *)MessageGetHeader(HEADER_NAME_CONTENT_LENGTH, message);
    
    ContentLengthHeaderSetLength(c, length);
}

void Message2String(char *result, struct Message *message)
{
    char *p = result;

    assert(message != NULL);
    assert(result != NULL);

    if (message->type == MESSAGE_TYPE_REQUEST) {
        p = RequestLine2String(p, MessageGetRequestLine(message));
    } else {
        p = StatusLine2String(p, MessageGetStatusLine(message));
    }
    
    RawHeaders2String(p, message->headers);

    return ;
}

//Only for debug, no unit test for this funciton.
void MessageDump(struct Message *message)
{
    char messageString[MAX_MESSAGE_LENGTH] = {0};

    printf("\n");
    Message2String(messageString, message);
    printf("%s\n",messageString);
}

BOOL RequestResponseMatched(struct Message *request, struct Message *response)
{
    assert (request != NULL);
    assert (response != NULL);
    return ViaHeaderBranchMatched((struct ViaHeader *)MessageGetHeader(HEADER_NAME_VIA, request),
                                  (struct ViaHeader *)MessageGetHeader(HEADER_NAME_VIA, response)) 
        && CSeqHeaderMethodMatched((struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, request),
                                   (struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, response));    
}

struct Message *CreateMessage () 
{ 
    struct Message *message = calloc(1,sizeof (struct Message));
    message->type = MESSAGE_TYPE_NONE;
    message->headers = CreateHeaders();
    return message;
}

void DestoryMessage (struct Message **message) 
{  
    assert(message != NULL);

    if ((*message) == ((void *)0)) return;

    if ((*message)->type == MESSAGE_TYPE_REQUEST) {
        DestoryRequestLine((*message)->rr.request);
    } else {
        DestoryStatusLine((*message)->rr.status);
    }

    RawDestoryHeaders((*message)->headers);
    free(*message);
    *message = NULL;
}
