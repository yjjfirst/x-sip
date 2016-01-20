#include <stdio.h>

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
#include "Provision.h"

struct RequestLine *BuildRequestLine(struct Dialog *dialog)
{
    struct UserAgent *ua = DialogGetUserAgent(dialog);
    struct URI *uri = CreateUri(URI_SCHEME_SIP, DialogGetToUser(dialog), UserAgentGetProxy(ua), 0);
    struct RequestLine  *r = CreateRequestLine(DialogGetRequestMethod(dialog), uri);
    return r;
}

struct Header *BuildViaHeader(struct Dialog *dialog)
{
    struct URI *uri = CreateUri("", "", GetLocalIpAddr(), LOCAL_PORT);
    struct ViaHeader *via = CreateViaHeader(uri);
    
    struct Parameters *ps = ViaHeaderGetParameters(via);
    AddParameter(ps, VIA_BRANCH_PARAMETER_NAME, "z9hG4bK1491280923"); 
    return (struct Header *)via;
}

struct Header *BuildFromHeader(struct Dialog *dialog)
{
    struct UserAgent *ua = DialogGetUserAgent(dialog);
    struct URI *uri = CreateUri(URI_SCHEME_SIP, UserAgentGetUserName(ua), UserAgentGetProxy(ua), 0);
    struct ContactHeader *from = CreateFromHeader();
    struct Parameters *ps = ContactHeaderGetParameters(from);

    AddParameter(ps, HEADER_PARAMETER_NAME_TAG, "1069855717");
    ContactHeaderSetUri(from, uri);
    
    return (struct Header *)from;
}

struct Header *BuildToHeader(struct Dialog *dialog)
{
    struct UserAgent *ua = DialogGetUserAgent(dialog);
    struct URI *uri = CreateUri(URI_SCHEME_SIP, DialogGetToUser(dialog), UserAgentGetProxy(ua), 0);
    struct ContactHeader *to = CreateToHeader();

    ContactHeaderSetUri(to, uri);
    return (struct Header *)to;
}

struct Header *BuildContactHeader(struct Dialog *dialog)
{
    struct UserAgent *ua = DialogGetUserAgent(dialog);
    struct URI *uri = CreateUri(URI_SCHEME_SIP, UserAgentGetUserName(ua), GetLocalIpAddr(), 0);
    UriAddParameter(uri, "line", "6c451db26592505");
    struct ContactHeader *contact = CreateContactHeader();

    ContactHeaderSetUri(contact, uri);
    return  (struct Header *)contact;
}

struct Header *BuildMaxForwardsHeader(struct Dialog *dialog)
{
    struct MaxForwardsHeader *mf = CreateMaxForwardsHeader();
    return  (struct Header *)mf;
}

struct Header *BuildCallIdHeader(struct Dialog *dialog)
{
    struct CallIdHeader *id = CreateCallIdHeader(GenerateCallIdString());
    return (struct Header *)id;
}

struct Header *BuildCSeqHeader(struct Dialog *dialog)
{
    SIP_METHOD method = DialogGetRequestMethod(dialog);
    struct CSeqHeader *cseq = CreateCSeqHeader(1, MethodMap2String(method));

    return (struct Header *)cseq;
}

struct Header *BuildExpiresHeader(struct Dialog *dialog)
{
    struct ExpiresHeader *e = CreateExpiresHeader(7200);
    
    return (struct Header *)e;
}

struct Header *BuildContentLengthHeader(struct Dialog *dialog)
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
    MessageAddHeader(message, BuildViaHeader(dialog));
    MessageAddHeader(message, BuildFromHeader(dialog));
    MessageAddHeader(message, BuildToHeader(dialog));
    MessageAddHeader(message, BuildCallIdHeader(dialog));
    MessageAddHeader(message, BuildContactHeader(dialog));
    MessageAddHeader(message, BuildMaxForwardsHeader(dialog));
    MessageAddHeader(message, BuildCSeqHeader(dialog));
    MessageAddHeader(message, BuildContentLengthHeader(dialog));

    return message;
}

struct Message *BuildBindingMessage(struct Dialog *dialog)
{
    struct Message *binding = BuildRequestMessage(dialog, SIP_METHOD_REGISTER);
    MessageAddHeader(binding, BuildExpiresHeader(dialog));

    return binding;
}

struct Message *BuildInviteMessage(struct Dialog *dialog)
{
    struct Message *invite = BuildRequestMessage(dialog, SIP_METHOD_INVITE);

    return invite;
}

struct Message *BuildAckMessage(struct Dialog *dialog)
{
    struct Message *ack = BuildRequestMessage(dialog, SIP_METHOD_ACK);

    return ack;
}

struct Message *BuildTryingMessage(struct Message *invite)
{
    struct Message *message = CreateMessage();

    struct StatusLine *status = CreateStatusLine(100); 
    MessageSetStatusLine(message, status);

    struct ViaHeader *via = ViaHeaderDup((struct ViaHeader *)MessageGetHeader(HEADER_NAME_VIA, invite));
    MessageAddHeader(message, (struct Header *)via);

    struct ContactHeader *from = 
        ContactHeaderDup((struct ContactHeader *)MessageGetHeader(HEADER_NAME_FROM, invite));
    MessageAddHeader(message, (struct Header *)from);

    struct ContactHeader *to =
        ContactHeaderDup((struct ContactHeader *)MessageGetHeader(HEADER_NAME_TO, invite));
    if (ContactHeaderGetParameter(to, HEADER_PARAMETER_NAME_TAG) == NULL) {
        ContactHeaderSetParameter(to, HEADER_PARAMETER_NAME_TAG, "1234567890");
    }
    MessageAddHeader(message, (struct Header *)to);

    struct CallIdHeader *callId = 
        CallIdHeaderDup((struct CallIdHeader *)MessageGetHeader(HEADER_NAME_CALLID, invite));
    MessageAddHeader(message, (struct Header *)callId);

    struct CSeqHeader *cseqId =              
        CSeqHeaderDup((struct CSeqHeader *)MessageGetHeader(HEADER_NAME_CSEQ, invite));
    MessageAddHeader(message, (struct Header *)cseqId);
    
    return message;  
}
