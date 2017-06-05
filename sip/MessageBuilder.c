#include <stdio.h>
#include <string.h>

#include "Maps.h"
#include "StatusLine.h"
#include "RequestLine.h"
#include "MessageBuilder.h"
#include "Messages.h"
#include "ContactHeader.h"
#include "ViaHeader.h"
#include "MaxForwardsHeader.h"
#include "CallIdHeader.h"
#include "CSeqHeader.h"
#include "ExpiresHeader.h"
#include "ContentLengthHeader.h"
#include "URI.h"
#include "Parameter.h"
#include "Header.h"
#include "Provision.h"
#include "WWW_AuthenticationHeader.h"

struct HeaderBuilderMap
{
    char *headerName;
    struct Header *(*buildRequestHeader)(MESSAGE *message, char *from, char *to, char *ipaddr);
    struct Header *(*buildResponseHeader)(struct Message *request);
};

struct Header *BuildRequestViaHeader(MESSAGE *message, char *from, char *to, char *ipaddr)
{
    URI *uri = CreateUri("", "", GetLocalIpAddr(), LOCAL_PORT);
    VIA_HEADER *via = CreateViaHeader(uri);    
    struct Parameters *ps = ViaHeaderGetParameters(via);
    char branch[32];

    AddParameter(ps, "rport", "");
    GenerateBranch(branch);
    AddParameter(ps, VIA_BRANCH_PARAMETER_NAME, branch); 

    return (struct Header *)via;
}

struct Header *BuildRequestFromHeader(MESSAGE *message, char *from, char *to, char *ipaddr)
{
    CONTACT_HEADER *fromHeader = CreateFromHeader();
    URI *uri = CreateUri(URI_SCHEME_SIP, from, ipaddr, 0);
    struct Parameters *ps = ContactHeaderGetParameters(fromHeader);
    char tag[MAX_TAG_LENGTH +1] = {0};
    
    GenerateTag(tag);

    AddParameter(ps, HEADER_PARAMETER_NAME_TAG, tag);
    ContactHeaderSetUri(fromHeader, uri);
    
    return (struct Header *)fromHeader;
}

struct Header *BuildRequestToHeader(MESSAGE *message, char *from, char *to, char *ipaddr)
{
    URI *uri = CreateUri(URI_SCHEME_SIP, to, ipaddr, 0);

    CONTACT_HEADER *toHeader = CreateToHeader();
    ContactHeaderSetUri(toHeader, uri);

    return (struct Header *)toHeader;
}

struct Header *BuildRequestContactHeader(MESSAGE *message, char *from, char *to, char *ipaddr)
{
    URI *uri = CreateUri(URI_SCHEME_SIP, from, GetLocalIpAddr(), 0);
    UriAddParameter(uri, "line", "6c451db26592505");
    CONTACT_HEADER *contact = CreateContactHeader();

    ContactHeaderSetUri(contact, uri);
    return  (struct Header *)contact;
}

struct Header *BuildRequestMaxForwardsHeader(MESSAGE *message, char *from, char *to, char *ipaddr)
{
    struct MaxForwardsHeader *mf = CreateMaxForwardsHeader();
    return  (struct Header *)mf;
}

struct Header *BuildRequestCallIdHeader(MESSAGE *message, char *from, char *to, char *ipaddr)
{    
    char idString[16] = {0};
    struct CallIdHeader *id = CreateCallIdHeader(idString);

    return (struct Header *)id;
}

struct Header *BuildRequestCSeqHeader(MESSAGE *message, char *from, char *to, char *ipaddr)
{
    SIP_METHOD method = MessageGetMethod(message);
    int cseqNumber;
    
    cseqNumber = CSeqGenerateSeq();
    struct CSeqHeader *cseq = CreateCSeqHeader(cseqNumber, MethodMap2String(method));

    return (struct Header *)cseq;
}

struct Header *BuildRequestExpiresHeader(MESSAGE *message)
{
    struct ExpiresHeader *e = CreateExpiresHeader(3600);
    
    return (struct Header *)e;
}

struct Header *BuildRequestContentLengthHeader(MESSAGE *message, char *from, char *to, char *ipaddr)
{
    struct ContentLengthHeader *c = CreateContentLengthHeader();

    return (struct Header *)c;
}

struct HeaderBuilderMap RequestHeaderBuilderMap[] = {
    {HEADER_NAME_VIA, BuildRequestViaHeader, NULL},
    {HEADER_NAME_FROM, BuildRequestFromHeader, NULL},
    {HEADER_NAME_TO, BuildRequestToHeader, NULL},
    {HEADER_NAME_CALLID, BuildRequestCallIdHeader, NULL},
    {HEADER_NAME_CONTACT, BuildRequestContactHeader, NULL},
    {HEADER_NAME_MAX_FORWARDS,BuildRequestMaxForwardsHeader, NULL},
    {HEADER_NAME_CSEQ, BuildRequestCSeqHeader, NULL},
    {HEADER_NAME_CONTENT_LENGTH, BuildRequestContentLengthHeader, NULL},
    {NULL, NULL, NULL},
};

MESSAGE *BuildRequest(SIP_METHOD method, char *from, char *to, char *ipaddr, int port)
{
    MESSAGE *message = CreateMessage();
    struct HeaderBuilderMap *p = RequestHeaderBuilderMap;
    URI *uri = NULL;

    if (method == SIP_METHOD_REGISTER)
        uri = CreateUri(URI_SCHEME_SIP, NULL, ipaddr, 0);
    else
        uri = CreateUri(URI_SCHEME_SIP, to, ipaddr, 0);
    
    SetMessageAddr(message, ipaddr);
    SetMessagePort(message, port);

    SetMessageType(message, MESSAGE_TYPE_REQUEST);
    SetMessageRequestLine(message, CreateRequestLine(method, uri));

    for ( ; p->headerName != NULL; p++ ) {
        MessageAddHeader(message, p->buildRequestHeader(message, from, to, ipaddr));
    }

    return message;
}


MESSAGE *BuildAddBindingMessage(char *from, char *to, char *ipaddr, int port)
{
    MESSAGE *binding = BuildRequest(SIP_METHOD_REGISTER, from, to,  ipaddr, port);    

    CONTACT_HEADER *toHeader = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_TO, binding);
    UriSetUser(ContactHeaderGetUri(toHeader),to);    

    MessageAddHeader(binding, BuildRequestExpiresHeader(binding));
    
    return binding;
}

MESSAGE *BuildRemoveBindingMessage(char *from, char *to, char *ipaddr, int port)
{
    MESSAGE *remove = BuildAddBindingMessage(from, to, ipaddr, port);
    struct ExpiresHeader *e = (struct ExpiresHeader *)MessageGetHeader(HEADER_NAME_EXPIRES, remove);
    
    ExpiresHeaderSetExpires(e, 0);

    return remove;
}

MESSAGE *BuildInviteMessage(char *from, char *to, char *ipaddr, int port)
{
    MESSAGE *invite = BuildRequest(SIP_METHOD_INVITE, from, to, ipaddr, port);
    struct ContactHeader *toHeader = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_TO, invite);
    struct RequestLine *rl = (struct RequestLine *)MessageGetRequestLine(invite);
    
    UriSetUser(RequestLineGetUri(rl), to);
    UriSetUser(ContactHeaderGetUri(toHeader), to);
    
    return invite;
}

MESSAGE *BuildAckMessage(struct Message *invite, char *ipaddr, int port)
{
    MESSAGE *ack = NULL;
    char *from = NULL;
    char *to = NULL;

    GetFromUser(invite, &from);
    GetToUser(invite, &to);

    ack = BuildRequest(SIP_METHOD_ACK, from, to, ipaddr, port);

    if (invite != NULL) {
        MessageSetCSeqNumber(ack, MessageGetCSeqNumber(invite));
        MessageSetFromTag(ack, MessageGetFromTag(invite));
        MessageSetCallId(ack, MessageGetCallId(invite));
    }
    
    return ack;
}

MESSAGE *BuildByeMessage(struct Message *invite, char *ipaddr, int port)
{
    char *from = NULL;
    char *to = NULL;

    GetFromUser(invite, &from);
    GetToUser(invite, &to);

    MESSAGE *bye = BuildRequest(SIP_METHOD_BYE, from, to, ipaddr, port);

    if (invite != NULL) {
        char *tag = MessageGetToTag(invite);
        MessageSetToTag(bye, tag);
    }
    return bye;
}

void AddAuthHeaderUri(MESSAGE *authMessage,struct AuthHeader *authHeader)
{
    struct RequestLine *rl = MessageGetRequestLine(authMessage);
    URI *uri = RequestLineGetUri(rl);
    char uriString[URI_STRING_MAX_LENGTH + 1] = {0};
    
    Uri2StringExt(uriString, uri);
    AuthHeaderSetParameter(authHeader, AUTH_HEADER_URI,uriString); 
}

struct AuthHeader *BuildAuthHeader(MESSAGE *authMessage, char *user, char *secret, MESSAGE *challenge)
{
    struct AuthHeader *authHeader = CreateAuthorizationHeader();
    char response[MD5_HASH_LENGTH + 1] = {0};

    AuthHeaderSetScheme(authHeader, DIGEST);
    AuthHeaderSetParameter(authHeader, AUTH_HEADER_USER_NAME, user);
    AddAuthHeaderUri(authMessage, authHeader);

    struct AuthHeader
        *challengeAuthHeader = (struct AuthHeader *)MessageGetHeader(HEADER_NAME_WWW_AUTHENTICATE, challenge);

    AuthHeaderSetParameter(
        authHeader,
        AUTH_HEADER_REALM,
        AuthHeaderGetParameter(challengeAuthHeader, AUTH_HEADER_REALM));

    AuthHeaderSetParameter(
        authHeader,
        AUTH_HEADER_NONCE,
        AuthHeaderGetParameter(challengeAuthHeader, AUTH_HEADER_NONCE));

    CalculateResponse(
        user,
        secret,
        AuthHeaderGetParameter(authHeader,AUTH_HEADER_URI),
        AuthHeaderGetParameter(challengeAuthHeader, AUTH_HEADER_REALM),
        AuthHeaderGetParameter(challengeAuthHeader, AUTH_HEADER_NONCE),
        response);

    AuthHeaderSetParameter(authHeader, AUTH_HEADER_RESPONSE, response);
    AuthHeaderSetParameter(authHeader, AUTH_HEADER_ALGORITHM, ALGORITHM_MD5);

    return authHeader;
}

MESSAGE *BuildAuthorizationMessage(MESSAGE *challenge, char *user, char *secret, char *ipaddr, int port)
{
    MESSAGE *message = BuildRequest(SIP_METHOD_REGISTER, user, secret, ipaddr, port);

    char *callid = MessageGetCallId(challenge);
    struct CallIdHeader *callidHeader = (struct CallIdHeader *)MessageGetHeader(HEADER_NAME_CALLID, message);
    CallIdHeaderSetID(callidHeader, callid);

    int cseq = MessageGetCSeqNumber(challenge);
    MessageSetCSeqNumber(message, cseq + 1);
    
    MessageAddHeader(message, (struct Header *)BuildAuthHeader(message, user, secret, challenge));

    return message;
}

struct Header *BuildResponseViaHeader(MESSAGE *request)
{
    VIA_HEADER *via = ViaHeaderDup((VIA_HEADER *)MessageGetHeader(HEADER_NAME_VIA, request));
    return (struct Header *)via;
}

struct Header *BuildResponseFromHeader(MESSAGE *request)
{
    CONTACT_HEADER *from = 
        ContactHeaderDup((CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_FROM, request));
    return (struct Header *)from;
}

struct Header *BuildResponseToHeader(MESSAGE *request)
{
    char tag[MAX_TAG_LENGTH] = {0};
    CONTACT_HEADER *to =
        ContactHeaderDup((CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_TO, request));

    GenerateTag(tag);
    
    if (ContactHeaderGetParameter(to, HEADER_PARAMETER_NAME_TAG) == NULL) {
        ContactHeaderSetParameter(to, HEADER_PARAMETER_NAME_TAG, tag);
    }
    
    return (struct Header *)to;
}

struct Header *BuildResponseCallIdHeader(MESSAGE *request)
{
    struct CallIdHeader *callId = 
        CallIdHeaderDup((struct CallIdHeader *)MessageGetHeader(HEADER_NAME_CALLID, request));
    return (struct Header *)callId;
}

struct Header *BuildResponseCSeqHeader(MESSAGE *request)
{
    struct CSeqHeader *cseq =              
        CSeqHeaderDup((struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, request));
    return (struct Header *)cseq;
}

struct Header *BuildResponseContactHeader(MESSAGE *request)
{
    char *from;
    CONTACT_HEADER *c = CreateContactHeader();

    GetFromUser(request, &from);
    URI *uri = CreateUri(URI_SCHEME_SIP, from, GetLocalIpAddr(), LOCAL_PORT);

    ContactHeaderSetUri(c, uri);

    return (struct Header *)c;
}

struct HeaderBuilderMap ResponseHeaderBuilderMap[] = {
    {HEADER_NAME_VIA, NULL, BuildResponseViaHeader},
    {HEADER_NAME_FROM, NULL, BuildResponseFromHeader},
    {HEADER_NAME_TO, NULL, BuildResponseToHeader},
    {HEADER_NAME_CALLID, NULL, BuildResponseCallIdHeader},
    {HEADER_NAME_CSEQ, NULL, BuildResponseCSeqHeader},
    {HEADER_NAME_CONTACT, NULL, BuildResponseContactHeader},
    {NULL, NULL, NULL},
};

struct IntStringMap statusCode2ReasePhraseMaps[] = {
    {STATUS_CODE_TRYING, REASON_PHRASE_TRYING},
    {STATUS_CODE_OK, REASON_PHRASE_OK},
    {STATUS_CODE_RINGING, REASON_PHRASE_RINGING},
    {STATUS_CODE_MOVED_PERMANENTLY, REASON_PHRASE_MOVED_PERMANENTLY},
    {STATUS_CODE_REQUEST_TERMINATED, REASON_PHRASE_REQUEST_TERMINATED},
    {-1, ""},
};

void AddResponseHeaders(MESSAGE *response, MESSAGE *request)
{
    struct HeaderBuilderMap *p = ResponseHeaderBuilderMap;
    for ( ;p->headerName != NULL; p ++) {
        MessageAddHeader(response, p->buildResponseHeader(request));
    }
}

MESSAGE *BuildResponse(MESSAGE *request, int statusCode)
{
    MESSAGE *response = NULL;
    struct StatusLine *status = NULL;

    assert(request != NULL);

    status = CreateStatusLine(statusCode, IntMap2String(statusCode,statusCode2ReasePhraseMaps)); 
    response = CreateMessage();

    SetMessageStatusLine(response, status);
    AddResponseHeaders(response, request);

    SetMessagePort(response, GetMessagePort(request));
    SetMessageAddr(response, GetMessageAddr(request));

    return response;    
}

MESSAGE *BuildAckMessageWithinClientTransaction(MESSAGE *invite, char *ipaddr, int port, char *to_tag)
{
    assert(invite != NULL);
    assert(ipaddr != NULL);
    
    MESSAGE *ack = CreateMessage();
    struct RequestLine *rl = RequestLineDup(MessageGetRequestLine(invite));
    
    SetMessageType(ack, MESSAGE_TYPE_REQUEST);
    RequestLineSetMethod(rl, SIP_METHOD_NAME_ACK);
    SetMessageRequestLine(ack, rl);
    AddResponseHeaders(ack, invite);

    if (to_tag != NULL) {
        struct ContactHeader *to = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_TO,ack);
        ContactHeaderSetParameter(to, "tag", to_tag);
    }

    struct CSeqHeader *cseq = (struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, ack);
    CSeqHeaderSetMethod(cseq, SIP_METHOD_NAME_ACK);

    SetMessageAddr(ack, ipaddr);
    SetMessagePort(ack, port);
    
    return ack;
}
