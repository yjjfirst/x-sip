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
#include "UserAgent.h"
#include "Dialog.h"
#include "DialogId.h"
#include "Provision.h"
#include "Transaction.h"
#include "WWW_AuthenticationHeader.h"
#include "Accounts.h"

struct HeaderBuilderMap
{
    char *headerName;
    struct Header *(*buildRequestHeader)();
    struct Header *(*buildResponseHeader)(struct Message *request);
};

struct RequestLine *BuildRequestLine(struct Dialog *dialog)
{
    struct UserAgent *ua = DialogGetUserAgent(dialog);
    URI *uri ;
    URI *remoteUri = UriDup(DialogGetRemoteUri(dialog));

    if (remoteUri == NULL) {
        uri = CreateUri(URI_SCHEME_SIP, NULL, UaGetProxy(ua), 0);
    } else {
        uri = remoteUri;
    }
    
    return  CreateRequestLine(DialogGetRequestMethod(dialog), uri);
}

struct Header *BuildRequestViaHeader(struct Dialog *dialog)
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

struct Header *BuildRequestFromHeader(struct Dialog *dialog)
{
    struct UserAgent *ua = DialogGetUserAgent(dialog);
    URI *uri = CreateUri(URI_SCHEME_SIP, UaGetUserName(ua), UaGetProxy(ua), 0);
    CONTACT_HEADER *from = CreateFromHeader();
    struct Parameters *ps = ContactHeaderGetParameters(from);
    char tag[MAX_TAG_LENGTH +1] = {0};

    if (strlen(DialogGetLocalTag(dialog)) == 0){
        GenerateTag(tag);
        DialogSetLocalTag(dialog, tag);
    } else {
        strcpy(tag, DialogGetLocalTag(dialog));
    }

    AddParameter(ps, HEADER_PARAMETER_NAME_TAG, tag);
    ContactHeaderSetUri(from, uri);
    
    return (struct Header *)from;
}

struct Header *BuildRequestToHeader(struct Dialog *dialog)
{
    struct UserAgent *ua = DialogGetUserAgent(dialog);
    URI *uri;
    URI *remoteUri = DialogGetRemoteUri(dialog);

    if (remoteUri != NULL)
        uri = UriDup(remoteUri);
    else
        uri = CreateUri(URI_SCHEME_SIP, "", UaGetProxy(ua), 0);

    CONTACT_HEADER *to = CreateToHeader();
    ContactHeaderSetUri(to, uri);

    return (struct Header *)to;
}

struct Header *BuildRequestContactHeader(struct Dialog *dialog)
{
    struct UserAgent *ua = DialogGetUserAgent(dialog);
    URI *uri = CreateUri(URI_SCHEME_SIP, UaGetUserName(ua), GetLocalIpAddr(), 0);
    UriAddParameter(uri, "line", "6c451db26592505");
    CONTACT_HEADER *contact = CreateContactHeader();

    ContactHeaderSetUri(contact, uri);
    return  (struct Header *)contact;
}

struct Header *BuildRequestMaxForwardsHeader(struct Dialog *dialog)
{
    struct MaxForwardsHeader *mf = CreateMaxForwardsHeader();
    return  (struct Header *)mf;
}

struct Header *BuildRequestCallIdHeader(struct Dialog *dialog)
{    
    struct CallIdHeader *id = CreateCallIdHeader(DialogGetCallId(dialog));

    return (struct Header *)id;
}

struct Header *BuildRequestCSeqHeader(struct Dialog *dialog)
{
    SIP_METHOD method = DialogGetRequestMethod(dialog);
    int cseqNumber;
    
    if (GetLocalSeqNumber(dialog) == EMPTY_DIALOG_SEQNUMBER) {
        cseqNumber = CSeqGenerateSeq();
    } else {
        if (DialogGetRequestMethod(dialog) != SIP_METHOD_ACK)
            cseqNumber = GetLocalSeqNumber(dialog) + 1;
        else
            cseqNumber = GetLocalSeqNumber(dialog);
    }
        
    SetLocalSeqNumber(dialog, cseqNumber);
    struct CSeqHeader *cseq = CreateCSeqHeader(cseqNumber, MethodMap2String(method));

    return (struct Header *)cseq;
}

struct Header *BuildRequestExpiresHeader(struct Dialog *dialog)
{
    struct ExpiresHeader *e = CreateExpiresHeader(3600);
    
    return (struct Header *)e;
}

struct Header *BuildRequestContentLengthHeader(struct Dialog *dialog)
{
    struct ContentLengthHeader *c = CreateContentLengthHeader();

    return (struct Header *)c;
}

void SetMessageDestIpaddr(MESSAGE *m, struct Dialog *dialog)
{
    if (dialog == NULL) return;

    struct UserAgent *ua = DialogGetUserAgent(dialog);
    if (ua == NULL) return;
                   
    struct Account *account = UaGetAccount(ua);
    if (account == NULL) return;

    MessageSetDestAddr(m, AccountGetRegistrar(account));
    MessageSetDestPort(m, AccountGetRegistrarPort(account));
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

MESSAGE *BuildRequest(struct Dialog *dialog, SIP_METHOD method)
{
    MESSAGE *message = CreateMessage();
    struct HeaderBuilderMap *p = RequestHeaderBuilderMap;

    SetMessageDestIpaddr(message, dialog);
    
    DialogSetRequestMethod(dialog, method);
    MessageSetType(message, MESSAGE_TYPE_REQUEST);
    MessageSetRequestLine(message, BuildRequestLine(dialog));

    for ( ; p->headerName != NULL; p++ ) {
        MessageAddHeader(message, p->buildRequestHeader(dialog));
    }

    return message;
}


void SetToHeaderUserName(MESSAGE *message, struct Dialog *dialog)
{
    CONTACT_HEADER *toHeader = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_TO, message);
    struct UserAgent *ua = DialogGetUserAgent(dialog);
    struct Account *account = UaGetAccount(ua);

    UriSetUser(ContactHeaderGetUri(toHeader), AccountGetUserName(account));    
}

MESSAGE *BuildAddBindingMessage(struct Dialog *dialog)
{
    MESSAGE *binding = BuildRequest(dialog, SIP_METHOD_REGISTER);    

    SetToHeaderUserName(binding, dialog);
    MessageAddHeader(binding, BuildRequestExpiresHeader(dialog));
    
    return binding;
}

MESSAGE *BuildRemoveBindingMessage(struct Dialog *dialog)
{
    MESSAGE *remove = BuildAddBindingMessage(dialog);
    struct ExpiresHeader *e = (struct ExpiresHeader *)MessageGetHeader(HEADER_NAME_EXPIRES, remove);
    
    ExpiresHeaderSetExpires(e, 0);

    return remove;
}

URI *BuildRemoteUri(struct Dialog *dialog, char *to)
{
    struct UserAgent *ua = DialogGetUserAgent(dialog);
    struct Account *account = UaGetAccount(ua);    
    URI *uri = CreateUri(URI_SCHEME_SIP, to, AccountGetProxyAddr(account), 0);

    return uri;
}

MESSAGE *BuildInviteMessage(struct Dialog *dialog, char *to)
{
    MESSAGE *invite = BuildRequest(dialog, SIP_METHOD_INVITE);
    struct ContactHeader *toHeader = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_TO, invite);
    struct RequestLine *rl = (struct RequestLine *)MessageGetRequestLine(invite);
    URI *uri = NULL;

    if (DialogGetRemoteUri(dialog) == NULL) {
        uri = BuildRemoteUri(dialog, to);        
        DialogSetRemoteUri(dialog, uri);
    }

    UriSetUser(RequestLineGetUri(rl), to);
    UriSetUser(ContactHeaderGetUri(toHeader), to);
    
    return invite;
}

MESSAGE *BuildAckMessage(struct Dialog *dialog)
{
    MESSAGE *ack = BuildRequest(dialog, SIP_METHOD_ACK);
    MessageSetRemoteTag(ack, DialogGetRemoteTag(dialog));
   
    return ack;
}

MESSAGE *BuildByeMessage(struct Dialog *dialog)
{
    MESSAGE *bye = BuildRequest(dialog, SIP_METHOD_BYE);
    CONTACT_HEADER *to = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_TO, bye);
    struct Parameters *ps = ContactHeaderGetParameters(to);

    AddParameter(ps, HEADER_PARAMETER_NAME_TAG, DialogGetRemoteTag(dialog));

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

struct AuthHeader *BuildAuthHeader(MESSAGE *authMessage, struct Dialog *dialog, MESSAGE *challenge)
{
    struct AuthHeader *authHeader = CreateAuthorizationHeader(dialog);
    struct UserAgent *ua = DialogGetUserAgent(dialog);
    struct Account *account = UaGetAccount(ua);
    char response[MD5_HASH_LENGTH + 1] = {0};

    AuthHeaderSetScheme(authHeader, DIGEST);
    AuthHeaderSetParameter(authHeader, AUTH_HEADER_USER_NAME, AccountGetUserName(account));
    AddAuthHeaderUri(authMessage, authHeader);

    struct AuthHeader
        *challengeAuthHeader = (struct AuthHeader *)MessageGetHeader(HEADER_NAME_WWW_AUTHENTICATE, challenge);

    AuthHeaderSetParameter(authHeader,
                           AUTH_HEADER_REALM,
                           AuthHeaderGetParameter(challengeAuthHeader, AUTH_HEADER_REALM));

    AuthHeaderSetParameter(authHeader,
                           AUTH_HEADER_NONCE,
                           AuthHeaderGetParameter(challengeAuthHeader, AUTH_HEADER_NONCE));

    CalculateResponse(AccountGetAuthName(account),
                      AccountGetPasswd(account),
                      AuthHeaderGetParameter(authHeader,AUTH_HEADER_URI),
                      AuthHeaderGetParameter(challengeAuthHeader, AUTH_HEADER_REALM),
                      AuthHeaderGetParameter(challengeAuthHeader, AUTH_HEADER_NONCE),
                      response);

    AuthHeaderSetParameter(authHeader, AUTH_HEADER_RESPONSE, response);
    AuthHeaderSetParameter(authHeader, AUTH_HEADER_ALGORITHM, ALGORITHM_MD5);

    return authHeader;
}

MESSAGE *BuildAuthorizationMessage(struct Dialog *dialog, MESSAGE *challenge)
{
    MESSAGE *message = BuildRequest(dialog, SIP_METHOD_REGISTER);

    char *callid = MessageGetCallId(challenge);
    struct CallIdHeader *callidHeader = (struct CallIdHeader *)MessageGetHeader(HEADER_NAME_CALLID, message);
    CallIdHeaderSetID(callidHeader, callid);

    SetToHeaderUserName(message, dialog);
    MessageAddHeader(message, (struct Header *)BuildAuthHeader(message,dialog, challenge));
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

void BuildResponseToTag(char *tag, MESSAGE *request)
{
    struct Dialog *dialog = NULL;    
    struct Transaction *t = MessageBelongTo(request);

    if (t == NULL) {
        GenerateTag(tag);
        return;
    }

    dialog = (struct Dialog *)GetTransactionUser(t);
    if (dialog == NULL) {
        GenerateTag(tag);
        return;        
    }

    if (strlen(DialogGetLocalTag(dialog)) != 0) {
        strcpy(tag, DialogGetLocalTag(dialog));
    } else {
        GenerateTag(tag);
        DialogSetLocalTag(dialog, tag);
    }
}

struct Header *BuildResponseToHeader(MESSAGE *request)
{
    char tag[MAX_TAG_LENGTH] = {0};
    CONTACT_HEADER *to =
        ContactHeaderDup((CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_TO, request));

    BuildResponseToTag(tag, request);
    
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
    CONTACT_HEADER *c = CreateContactHeader();
    URI *uri = CreateUri(URI_SCHEME_SIP, "88001", GetLocalIpAddr(), LOCAL_PORT);

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

void AddResponseHeaders(MESSAGE *response, MESSAGE *request)
{
    struct HeaderBuilderMap *p = ResponseHeaderBuilderMap;
    for ( ;p->headerName != NULL; p ++) {
        MessageAddHeader(response, p->buildResponseHeader(request));
    }
}

struct IntStringMap statusCode2ReasePhraseMaps[] = {
    {STATUS_CODE_TRYING, REASON_PHRASE_TRYING},
    {STATUS_CODE_OK, REASON_PHRASE_OK},
    {STATUS_CODE_RINGING, REASON_PHRASE_RINGING},
    {STATUS_CODE_MOVED_PERMANENTLY, REASON_PHRASE_MOVED_PERMANENTLY},
    {-1, ""},
};

MESSAGE *BuildResponse(struct Dialog *dialog, MESSAGE *invite, int statusCode)
{
    MESSAGE *response = NULL;
    struct StatusLine *status = NULL;

    assert(invite != NULL);

    status = CreateStatusLine(statusCode, IntMap2String(statusCode,statusCode2ReasePhraseMaps)); 
    response = CreateMessage();

    MessageSetStatusLine(response, status);
    AddResponseHeaders(response, invite);

    SetMessageDestIpaddr(response, dialog); 

    return response;    
}

MESSAGE *BuildAckMessageWithinClientTransaction(MESSAGE *invite)
{
    assert(invite != NULL);

    MESSAGE *ack = CreateMessage();
    struct RequestLine *rl = RequestLineDup(MessageGetRequestLine(invite));
    
    MessageSetType(ack, MESSAGE_TYPE_REQUEST);
    RequestLineSetMethod(rl, SIP_METHOD_NAME_ACK);
    MessageSetRequestLine(ack, rl);
    AddResponseHeaders(ack, invite);
    
    struct CSeqHeader *cseq = (struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, ack);
    CSeqHeaderSetMethod(cseq, SIP_METHOD_NAME_ACK);

    return ack;
}
