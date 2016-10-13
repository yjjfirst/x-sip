#include <stdio.h>
#include <string.h>

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

struct RequestLine *BuildRequestLine(struct Dialog *dialog)
{
    struct UserAgent *ua = DialogGetUserAgent(dialog);
    URI *uri ;
    URI *remoteUri = UriDup(DialogGetRemoteUri(dialog));

    if (remoteUri == NULL) {
        uri = CreateUri(URI_SCHEME_SIP, NULL, UserAgentGetProxy(ua), 0);
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
    URI *uri = CreateUri(URI_SCHEME_SIP, UserAgentGetUserName(ua), UserAgentGetProxy(ua), 0);
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
        uri = CreateUri(URI_SCHEME_SIP, "", UserAgentGetProxy(ua), 0);

    CONTACT_HEADER *to = CreateToHeader();
    ContactHeaderSetUri(to, uri);

    return (struct Header *)to;
}

struct Header *BuildRequestContactHeader(struct Dialog *dialog)
{
    struct UserAgent *ua = DialogGetUserAgent(dialog);
    URI *uri = CreateUri(URI_SCHEME_SIP, UserAgentGetUserName(ua), GetLocalIpAddr(), 0);
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
    
    if (DialogGetLocalSeqNumber(dialog) == EMPTY_DIALOG_SEQNUMBER) {
        cseqNumber = CSeqGenerateSeq();
    } else {
        if (DialogGetRequestMethod(dialog) != SIP_METHOD_ACK)
            cseqNumber = DialogGetLocalSeqNumber(dialog) + 1;
        else
            cseqNumber = DialogGetLocalSeqNumber(dialog);
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

void BuildMessageDest(MESSAGE *m, struct Dialog *dialog)
{
    if (dialog == NULL) return;

    struct UserAgent *ua = DialogGetUserAgent(dialog);
    if (ua == NULL) return;
                   
    struct Account *account = UserAgentGetAccount(ua);
    if (account == NULL) return;

    MessageSetDestAddr(m, AccountGetRegistrar(account));
    MessageSetDestPort(m, AccountGetRegistrarPort(account));
}

MESSAGE *BuildRequestMessage(struct Dialog *dialog, SIP_METHOD method)
{
    MESSAGE *message = CreateMessage();

    BuildMessageDest(message, dialog);
    
    DialogSetRequestMethod(dialog, method);
    MessageSetType(message, MESSAGE_TYPE_REQUEST);
    MessageSetRequestLine(message, BuildRequestLine(dialog));
    MessageAddHeader(message, BuildRequestViaHeader(dialog));
    MessageAddHeader(message, BuildRequestFromHeader(dialog));
    MessageAddHeader(message, BuildRequestToHeader(dialog));
    MessageAddHeader(message, BuildRequestCallIdHeader(dialog));
    MessageAddHeader(message, BuildRequestContactHeader(dialog));
    MessageAddHeader(message, BuildRequestMaxForwardsHeader(dialog));
    MessageAddHeader(message, BuildRequestCSeqHeader(dialog));
    MessageAddHeader(message, BuildRequestContentLengthHeader(dialog));

    return message;
}


void SetToHeaderUserName(MESSAGE *message, struct Dialog *dialog)
{
    CONTACT_HEADER *toHeader = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_TO, message);
    struct UserAgent *ua = DialogGetUserAgent(dialog);
    struct Account *account = UserAgentGetAccount(ua);

    UriSetUser(ContactHeaderGetUri(toHeader), AccountGetUserName(account));    
}

MESSAGE *BuildAddBindingMessage(struct Dialog *dialog)
{
    MESSAGE *binding = BuildRequestMessage(dialog, SIP_METHOD_REGISTER);    

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
    struct Account *account = UserAgentGetAccount(ua);    
    URI *uri = CreateUri(URI_SCHEME_SIP, to, AccountGetProxyAddr(account), 0);

    return uri;
}

MESSAGE *BuildInviteMessage(struct Dialog *dialog, char *to)
{
    MESSAGE *invite = BuildRequestMessage(dialog, SIP_METHOD_INVITE);
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
    MESSAGE *ack = BuildRequestMessage(dialog, SIP_METHOD_ACK);
    MessageSetRemoteTag(ack, DialogGetRemoteTag(dialog));
   
    return ack;
}

MESSAGE *BuildByeMessage(struct Dialog *dialog)
{
    MESSAGE *bye = BuildRequestMessage(dialog, SIP_METHOD_BYE);
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
    struct Account *account = UserAgentGetAccount(ua);
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
    MESSAGE *message = BuildRequestMessage(dialog, SIP_METHOD_REGISTER);

    char *callid = MessageGetCallId(challenge);
    struct CallIdHeader *callidHeader = (struct CallIdHeader *)MessageGetHeader(HEADER_NAME_CALLID, message);
    CallIdHeaderSetID(callidHeader, callid);

    SetToHeaderUserName(message, dialog);
    MessageAddHeader(message, (struct Header *)BuildAuthHeader(message,dialog, challenge));
    return message;
}

void AddResponseViaHeader(MESSAGE *response, MESSAGE *invite)
{
    VIA_HEADER *via = ViaHeaderDup((VIA_HEADER *)MessageGetHeader(HEADER_NAME_VIA, invite));
    MessageAddHeader(response, (HEADER *)via);
}

void AddResponseFromHeader(MESSAGE *response, MESSAGE *invite)
{
    CONTACT_HEADER *from = 
        ContactHeaderDup((CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_FROM, invite));
    MessageAddHeader(response, (struct Header *)from);
}

void BuildResponseToTag(char *tag, MESSAGE *invite)
{
    struct Dialog *dialog = NULL;    
    struct Transaction *t = MessageBelongTo(invite);

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

void AddResponseToHeader(MESSAGE *response, MESSAGE *invite)
{
    char tag[MAX_TAG_LENGTH] = {0};
    CONTACT_HEADER *to =
        ContactHeaderDup((CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_TO, invite));

    BuildResponseToTag(tag, invite);
    
    if (ContactHeaderGetParameter(to, HEADER_PARAMETER_NAME_TAG) == NULL) {
        ContactHeaderSetParameter(to, HEADER_PARAMETER_NAME_TAG, tag);
    }
    
    MessageAddHeader(response, (struct Header *)to);
}

void AddResponseCallIdHeader(MESSAGE *response, MESSAGE *invite)
{
    struct CallIdHeader *callId = 
        CallIdHeaderDup((struct CallIdHeader *)MessageGetHeader(HEADER_NAME_CALLID, invite));
    MessageAddHeader(response, (struct Header *)callId);
}

void AddResponseCSeqHeader(MESSAGE *response, MESSAGE *invite)
{
    struct CSeqHeader *cseqId =              
        CSeqHeaderDup((struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, invite));
    MessageAddHeader(response, (struct Header *)cseqId);

}

void AddResponseContactHeader(MESSAGE *response, MESSAGE *invite)
{
    CONTACT_HEADER *c = CreateContactHeader();
    URI *uri = CreateUri(URI_SCHEME_SIP, "88001", GetLocalIpAddr(), LOCAL_PORT);

    ContactHeaderSetUri(c, uri);
    MessageAddHeader(response, (struct Header *)c);
    
}

void AddResponseHeaders(MESSAGE *response, MESSAGE *request)
{
    AddResponseViaHeader(response, request);
    AddResponseFromHeader(response, request);
    AddResponseToHeader(response, request);
    AddResponseCallIdHeader(response, request);
    AddResponseCSeqHeader(response, request);
    AddResponseContactHeader(response, request);
}

MESSAGE *BuildResponseMessage(MESSAGE *request, struct StatusLine *status)
{
    assert(request != NULL);
    
    MESSAGE *message = CreateMessage();
    MessageSetStatusLine(message, status);
    AddResponseHeaders(message, request);

    return message;  
}

MESSAGE *BuildTryingMessage(struct Dialog *dialog, MESSAGE *invite)
{
    MESSAGE *trying = NULL;
    assert(invite != NULL);

    struct StatusLine *status = CreateStatusLine(STATUS_CODE_TRYING, REASON_PHRASE_TRYING); 
    trying = BuildResponseMessage(invite, status);
    BuildMessageDest(trying, dialog); 

    return trying;
}

MESSAGE *BuildRingingMessage(struct Dialog *dialog, MESSAGE *invite)
{
    assert(invite != NULL);

    MESSAGE *ringing = NULL;
    struct StatusLine *status = CreateStatusLine(STATUS_CODE_RINGING, REASON_PHRASE_RINGING); 
    ringing = BuildResponseMessage(invite, status);
    BuildMessageDest(ringing, dialog); 

    return ringing;
}

MESSAGE *Build200OkMessage(struct Dialog *dialog, MESSAGE *request)
{
    assert(request != NULL);

    MESSAGE *ok;
    struct StatusLine *status = CreateStatusLine(STATUS_CODE_OK, REASON_PHRASE_OK);

    ok =  BuildResponseMessage(request, status);
    BuildMessageDest(ok, dialog);
    
    return ok;
}

MESSAGE *Build301Message(MESSAGE *invite)
{
    assert(invite != NULL);

    struct StatusLine *status = CreateStatusLine(STATUS_CODE_MOVED_PERMANENTLY, REASON_PHRASE_MOVED_PERMANENTLY);
    return BuildResponseMessage(invite, status);
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
