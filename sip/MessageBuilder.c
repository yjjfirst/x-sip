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

void AddRequestLine(struct Message *m)
{
    struct URI *uri = CreateUri(URI_SCHEME_SIP, "", PROXY_IPADDR, 0);
    struct RequestLine  *r = CreateRequestLine(SIP_METHOD_REGISTER, uri);
    MessageSetRequest(m, r);
}

void AddViaHeader(struct Message *m)
{
    struct URI *uri = CreateUri(URI_SCHEME_SIP, "", LOCAL_IPADDR, LOCAL_IPPORT);
    struct ViaHeader *via = CreateViaHeader(uri);
    
    MessageAddHeader(m, (struct Header *)via);
}

void AddFromHeader(struct Message *m )
{
    struct URI *uri = CreateUri(URI_SCHEME_SIP, USER_NAME, PROXY_IPADDR, 0);
    struct ContactHeader *from = CreateFromHeader();

    ContactHeaderSetDisplayName(from, USER_NAME);
    ContactHeaderSetUri(from, uri);
    MessageAddHeader(m, (struct Header *)from);
}

void AddToHeader(struct Message *m)
{
    struct URI *uri = CreateUri(URI_SCHEME_SIP, USER_NAME, PROXY_IPADDR, 0);
    struct ContactHeader *to = CreateToHeader();

    ContactHeaderSetDisplayName(to, USER_NAME);
    ContactHeaderSetUri(to, uri);
    MessageAddHeader(m, (struct Header *)to);
}

void AddContactHeader(struct Message *m)
{
    struct URI *uri = CreateUri(URI_SCHEME_SIP, USER_NAME, LOCAL_IPADDR, 0);
    struct ContactHeader *contact = CreateContactHeader();

    ContactHeaderSetUri(contact, uri);
    MessageAddHeader(m, (struct Header *)contact);
}

void AddMaxForwardsHeader(struct Message *m)
{
    struct MaxForwardsHeader *mf = CreateMaxForwardsHeader();
    MessageAddHeader(m, (struct Header *)mf);
}

void AddCallIdHeader(struct Message *m)
{
    struct CallIdHeader *id = CreateCallIdHeader();
    
    MessageAddHeader(m, (struct Header *)id);
}

void AddCSeqHeader(struct Message *m)
{
    struct CSeqHeader *cseq = CreateCSeqHeader();

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

struct Message *BuildRegisterMessage()
{
    struct Message *m = CreateMessage();
    
    AddRequestLine(m);
    AddViaHeader(m);
    AddFromHeader(m);
    AddToHeader(m);
    AddCallIdHeader(m);
    AddCSeqHeader(m);
    AddContactHeader(m);
    AddMaxForwardsHeader(m);
    AddExpiresHeader(m);
    AddContentLengthHeader(m);
    return m;
}

