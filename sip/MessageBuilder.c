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
    struct URI *uri ;
    struct URI *remoteTarget = UriDup(DialogGetRemoteTarget(dialog));

    if (remoteTarget == NULL) {
        if (DialogGetRequestMethod(dialog) == SIP_METHOD_REGISTER)
            uri = CreateUri(URI_SCHEME_SIP, NULL, UserAgentGetProxy(ua), 0);
        else 
            uri = CreateUri(URI_SCHEME_SIP, DialogGetToUser(dialog), UserAgentGetProxy(ua), 0);
    } else {
        uri = remoteTarget;
    }
    
    return  CreateRequestLine(DialogGetRequestMethod(dialog), uri);
}

struct Header *BuildRequestViaHeader(struct Dialog *dialog)
{
    struct URI *uri = CreateUri("", "", GetLocalIpAddr(), LOCAL_PORT);
    struct ViaHeader *via = CreateViaHeader(uri);    
    struct Parameters *ps = ViaHeaderGetParameters(via);
    char branch[32];

    GenerateBranch(branch);
    AddParameter(ps, VIA_BRANCH_PARAMETER_NAME, branch); 

    return (struct Header *)via;
}

struct Header *BuildRequestFromHeader(struct Dialog *dialog)
{
    struct UserAgent *ua = DialogGetUserAgent(dialog);
    struct URI *uri = CreateUri(URI_SCHEME_SIP, UserAgentGetUserName(ua), UserAgentGetProxy(ua), 0);
    struct ContactHeader *from = CreateFromHeader();
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
    struct URI *uri;
    struct URI *remoteUri = DialogGetRemoteUri(dialog);

    if (remoteUri != NULL)
        uri = remoteUri;
    else
        uri = CreateUri(URI_SCHEME_SIP, DialogGetToUser(dialog), UserAgentGetProxy(ua), 0);

    struct ContactHeader *to = CreateToHeader();
    ContactHeaderSetUri(to, uri);

    return (struct Header *)to;
}

struct Header *BuildRequestContactHeader(struct Dialog *dialog)
{
    struct UserAgent *ua = DialogGetUserAgent(dialog);
    struct URI *uri = CreateUri(URI_SCHEME_SIP, UserAgentGetUserName(ua), GetLocalIpAddr(), 0);
    UriAddParameter(uri, "line", "6c451db26592505");
    struct ContactHeader *contact = CreateContactHeader();

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
    struct CSeqHeader *cseq = CreateCSeqHeader(1, MethodMap2String(method));

    return (struct Header *)cseq;
}

struct Header *BuildRequestExpiresHeader(struct Dialog *dialog)
{
    struct ExpiresHeader *e = CreateExpiresHeader(7200);
    
    return (struct Header *)e;
}

struct Header *BuildRequestContentLengthHeader(struct Dialog *dialog)
{
    struct ContentLengthHeader *c = CreateContentLengthHeader();

    return (struct Header *)c;
}

struct Message *BuildRequestMessage(struct Dialog *dialog, SIP_METHOD method)
{
    struct Message *message = CreateMessage();
 
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

struct Message *BuildAddBindingMessage(struct Dialog *dialog)
{
    struct Message *binding = BuildRequestMessage(dialog, SIP_METHOD_REGISTER);
    MessageAddHeader(binding, BuildRequestExpiresHeader(dialog));

    return binding;
}

struct Message *BuildRemoveBindingMessage(struct Dialog *dialog)
{
    struct Message *remove = BuildAddBindingMessage(dialog);
    struct ExpiresHeader *e = (struct ExpiresHeader *)MessageGetHeader(HEADER_NAME_EXPIRES, remove);
    
    ExpiresHeaderSetExpires(e, 0);

    return remove;
}

struct Message *BuildInviteMessage(struct Dialog *dialog)
{
    struct Message *invite = BuildRequestMessage(dialog, SIP_METHOD_INVITE);

    return invite;
}

struct Message *BuildAckMessage(struct Dialog *dialog)
{
    struct Message *ack = BuildRequestMessage(dialog, SIP_METHOD_ACK);
    MessageSetRemoteTag(ack, DialogGetRemoteTag(dialog));
    return ack;
}

struct Message *BuildByeMessage(struct Dialog *dialog)
{
    struct Message *bye = BuildRequestMessage(dialog, SIP_METHOD_BYE);
    struct ContactHeader *to = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_TO, bye);
    struct Parameters *ps = ContactHeaderGetParameters(to);

    AddParameter(ps, HEADER_PARAMETER_NAME_TAG, DialogGetRemoteTag(dialog));

    return bye;
}

void AddAuthHeaderUri(struct Message *authMessage,struct AuthHeader *authHeader)
{
    struct RequestLine *rl = MessageGetRequestLine(authMessage);
    struct URI *uri = RequestLineGetUri(rl);
    char uriString[URI_STRING_MAX_LENGTH + 1] = {0};
    
    Uri2StringExt(uriString, uri);
    AuthHeaderSetParameter(authHeader, AUTH_HEADER_URI,uriString); 
}

struct AuthHeader *BuildAuthHeader(struct Message *authMessage, struct Dialog *dialog, struct Message *challenge)
{
    struct AuthHeader *authHeader = CreateAuthorizationHeader(dialog);
    struct UserAgent *ua = DialogGetUserAgent(dialog);
    struct Account *account = UserAgentGetAccount(ua);
    char response[MD5_HASH_LENGTH + 1] = {0};

    AuthHeaderSetScheme(authHeader, DIGEST);
    AuthHeaderSetParameter(authHeader, AUTH_HEADER_USER_NAME, AccountGetUserName(account));
    AuthHeaderSetParameter(authHeader, AUTH_HEADER_ALGORITHM, ALGORITHM_MD5);
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
                       
    return authHeader;
}

struct Message *BuildAuthorizationMessage(struct Dialog *dialog, struct Message *challenge)
{
    struct Message *message = BuildRequestMessage(dialog, SIP_METHOD_REGISTER);
   
    MessageAddHeader(message, (struct Header *)BuildAuthHeader(message,dialog, challenge));
    return message;
}

void AddResponseViaHeader(struct Message *response, struct Message *invite)
{
    VIA_HEADER *via = ViaHeaderDup((VIA_HEADER *)MessageGetHeader(HEADER_NAME_VIA, invite));
    MessageAddHeader(response, (HEADER *)via);
}

void AddResponseFromHeader(struct Message *response, struct Message *invite)
{
    struct ContactHeader *from = 
        ContactHeaderDup((struct ContactHeader *)MessageGetHeader(HEADER_NAME_FROM, invite));
    MessageAddHeader(response, (struct Header *)from);
}

void BuildResponseToTag(char *tag, struct Message *invite)
{
    struct Dialog *dialog = NULL;    
    struct Transaction *t = MessageBelongTo(invite);

    if (t == NULL) {
        GenerateTag(tag);
        return;
    }

    dialog = (struct Dialog *)TransactionGetUser(t);
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

void AddResponseToHeader(struct Message *response, struct Message *invite)
{
    char tag[MAX_TAG_LENGTH] = {0};
    struct ContactHeader *to =
        ContactHeaderDup((struct ContactHeader *)MessageGetHeader(HEADER_NAME_TO, invite));

    BuildResponseToTag(tag, invite);
    
    if (ContactHeaderGetParameter(to, HEADER_PARAMETER_NAME_TAG) == NULL) {
        ContactHeaderSetParameter(to, HEADER_PARAMETER_NAME_TAG, tag);
    }
    
    MessageAddHeader(response, (struct Header *)to);
}

void AddResponseCallIdHeader(struct Message *response, struct Message *invite)
{
    struct CallIdHeader *callId = 
        CallIdHeaderDup((struct CallIdHeader *)MessageGetHeader(HEADER_NAME_CALLID, invite));
    MessageAddHeader(response, (struct Header *)callId);
}

void AddResponseCSeqHeader(struct Message *response, struct Message *invite)
{
    struct CSeqHeader *cseqId =              
        CSeqHeaderDup((struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, invite));
    MessageAddHeader(response, (struct Header *)cseqId);

}

void AddResponseContactHeader(struct Message *response, struct Message *invite)
{
    struct ContactHeader *c = CreateContactHeader();
    struct URI *uri = CreateUri(URI_SCHEME_SIP, "88001", GetLocalIpAddr(), LOCAL_PORT);

    ContactHeaderSetUri(c, uri);
    MessageAddHeader(response, (struct Header *)c);
    
}

void AddResponseHeaders(struct Message *response, struct Message *request)
{
    AddResponseViaHeader(response, request);
    AddResponseFromHeader(response, request);
    AddResponseToHeader(response, request);
    AddResponseCallIdHeader(response, request);
    AddResponseCSeqHeader(response, request);
    AddResponseContactHeader(response, request);
}

struct Message *BuildResponseMessage(struct Message *request, struct StatusLine *status)
{
    assert(request != NULL);
    
    struct Message *message = CreateMessage();
    MessageSetStatusLine(message, status);
    AddResponseHeaders(message, request);

    return message;  
}

struct Message *BuildTryingMessage(struct Message *invite)
{
    assert(invite != NULL);

    struct StatusLine *status = CreateStatusLine(STATUS_CODE_TRYING, REASON_PHRASE_TRYING); 
    return BuildResponseMessage(invite, status);
}

struct Message *BuildRingingMessage(struct Message *invite)
{
    assert(invite != NULL);

    struct StatusLine *status = CreateStatusLine(STATUS_CODE_RINGING, REASON_PHRASE_RINGING); 
    return BuildResponseMessage(invite, status);
}

struct Message *Build200OkMessage(struct Message *request)
{
    assert(request != NULL);

    struct StatusLine *status = CreateStatusLine(STATUS_CODE_OK, REASON_PHRASE_OK);
    return BuildResponseMessage(request, status);
}

struct Message *Build301Message(struct Message *invite)
{
    assert(invite != NULL);

    struct StatusLine *status = CreateStatusLine(STATUS_CODE_MOVED_PERMANENTLY, REASON_PHRASE_MOVED_PERMANENTLY);
    return BuildResponseMessage(invite, status);
}

struct Message *BuildAckMessageWithinClientTransaction(struct Message *invite)
{
    assert(invite != NULL);

    struct Message *ack = CreateMessage();
    struct RequestLine *rl = RequestLineDup(MessageGetRequestLine(invite));
    
    MessageSetType(ack, MESSAGE_TYPE_REQUEST);
    RequestLineSetMethod(rl, SIP_METHOD_NAME_ACK);
    MessageSetRequestLine(ack, rl);
    AddResponseHeaders(ack, invite);
    
    struct CSeqHeader *cseq = (struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, ack);
    CSeqHeaderSetMethod(cseq, SIP_METHOD_NAME_ACK);

    return ack;
}
