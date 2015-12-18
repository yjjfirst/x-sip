#include <stdio.h>

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

void AddRequestLine(struct Message *m, char *proxy, SIP_METHOD method, char *user)
{
    struct URI *uri = CreateUri(URI_SCHEME_SIP, user, proxy, 0);
    struct RequestLine  *r = CreateRequestLine(method, uri);
    MessageSetRequest(m, r);
}

void AddViaHeader(struct Message *m)
{
    struct URI *uri = CreateUri("", "", LOCAL_IPADDR, LOCAL_PORT);
    struct ViaHeader *via = CreateViaHeader(uri);
    
    struct Parameters *ps = ViaHeaderGetParameters(via);
    AddParameter(ps, VIA_BRANCH_PARAMETER_NAME, "z9hG4bK1491280923"); 

    MessageAddHeader(m, (struct Header *)via);
}

void AddFromHeader(struct Message *m, char *proxy, char *user )
{
    struct URI *uri = CreateUri(URI_SCHEME_SIP, user, proxy, 0);
    struct ContactHeader *from = CreateFromHeader();
    struct Parameters *ps = ContactHeaderGetParameters(from);

    AddParameter(ps, HEADER_PARAMETER_NAME_TAG, "1069855717");
    ContactHeaderSetUri(from, uri);
    MessageAddHeader(m, (struct Header *)from);
}

void AddToHeader(struct Message *m, char *proxy, char *user)
{
    struct URI *uri = CreateUri(URI_SCHEME_SIP, user, proxy, 0);
    struct ContactHeader *to = CreateToHeader();

    ContactHeaderSetUri(to, uri);
    MessageAddHeader(m, (struct Header *)to);
}

void AddContactHeader(struct Message *m, char *user)
{
    struct URI *uri = CreateUri(URI_SCHEME_SIP, user, LOCAL_IPADDR, 0);
    UriAddParameter(uri, "line", "6c451db26592505");
    struct ContactHeader *contact = CreateContactHeader();

    ContactHeaderSetUri(contact, uri);
    MessageAddHeader(m, (struct Header *)contact);
}

void AddMaxForwardsHeader(struct Message *m)
{
    struct MaxForwardsHeader *mf = CreateMaxForwardsHeader();
    MessageAddHeader(m, (struct Header *)mf);
}

void AddCallIdHeader(struct Message *m, char *idString)
{
    struct CallIdHeader *id = CreateCallIdHeader(idString);
    MessageAddHeader(m, (struct Header *)id);
}

void AddCSeqHeader(struct Message *m, SIP_METHOD method)
{
    struct CSeqHeader *cseq = CreateCSeqHeader(1, MethodMap2String(method));

    MessageAddHeader(m, (struct Header *)cseq);
}

void AddExpiresHeader(struct Message *m)
{
    struct ExpiresHeader *e = CreateExpiresHeader(7200);
    
    MessageAddHeader(m, (struct Header *)e);
}

void AddContentLengthHeader(struct Message *m)
{
    struct ContentLengthHeader *c = CreateContentLengthHeader();

    MessageAddHeader(m, (struct Header *)c);
}

struct Message *BuildMessageTemplate(struct UserAgent *ua, SIP_METHOD method)
{
    struct Message *m = CreateMessage();

    AddViaHeader(m);
    AddFromHeader(m, UserAgentGetProxy(ua), UserAgentGetUserName(ua));
    AddCallIdHeader(m, GenerateCallIdString());
    AddContactHeader(m, UserAgentGetUserName(ua));
    AddMaxForwardsHeader(m);
    AddCSeqHeader(m, method);
    AddContentLengthHeader(m);

    return m;
}

struct Message *BuildBindingMessage(struct Dialog *dialog)
{
    struct UserAgent *ua = DialogGetUserAgent(dialog);
    struct Message *m = BuildMessageTemplate(ua, SIP_METHOD_REGISTER);

    AddRequestLine(m, UserAgentGetProxy(ua), SIP_METHOD_REGISTER, NULL);
    AddToHeader(m, UserAgentGetProxy(ua), UserAgentGetUserName(ua));
    AddExpiresHeader(m);

    return m;
}

struct Message *BuildInviteMessage(struct Dialog *dialog)
{
    struct UserAgent *ua = DialogGetUserAgent(dialog);
    char *to = DialogGetToUser(dialog);
    struct Message *invite = BuildMessageTemplate(ua, SIP_METHOD_INVITE);

    AddRequestLine(invite, UserAgentGetProxy(ua), SIP_METHOD_INVITE, to);
    AddToHeader(invite, UserAgentGetProxy(ua), to);

    return invite;
}

struct Message *BuildAckMessage(struct Dialog *dialog)
{
    struct UserAgent *ua = DialogGetUserAgent(dialog);
    struct Message *ack = BuildMessageTemplate(ua, SIP_METHOD_ACK);

    AddRequestLine(ack, UserAgentGetProxy(ua), SIP_METHOD_ACK, DialogGetToUser(dialog));

    return ack;
}
