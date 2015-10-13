#include "Messages.h"
#include "ContactHeader.h"
#include "ViaHeader.h"
#include "MaxForwardsHeader.h"
#include "CallIdHeader.h"
#include "CSeqHeader.h"
#include "ExpiresHeader.h"
#include "ContentLengthHeader.h"

void AddRequestLine(struct Message *m, char *user, char *host, int port)
{
    struct URI *uri = CreateUri("sip", user, host, port);
    struct RequestLine  *r = CreateRequestLine(SIP_METHOD_REGISTER, uri);
    MessageSetRequest(m, r);
}

void AddViaHeader(struct Message *m, char *uri)
{
    struct ViaHeader *via = CreateViaHeader(uri);
    
    MessageAddHeader(m, (struct Header *)via);
}

void AddFromHeader(struct Message *m, char *user, char *host, int port)
{
    struct URI *uri = CreateUri("sip", user, host, port);
    struct ContactHeader *from = CreateFromHeader();

    ContactHeaderSetDisplayName(from, "Martin Yang");
    ContactHeaderSetUri(from, uri);
    MessageAddHeader(m, (struct Header *)from);
}

void AddToHeader(struct Message *m, char *user, char *host, int port)
{
    struct URI *uri = CreateUri("sip", user, host, port);
    struct ContactHeader *to = CreateToHeader();

    ContactHeaderSetDisplayName(to, "Martin Yang");
    ContactHeaderSetUri(to, uri);
    MessageAddHeader(m, (struct Header *)to);
}

void AddContactHeader(struct Message *m, char *usr, char *host, int port)
{
    struct URI *uri = CreateUri("sip", usr, host, port);
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
    struct ExpiresHeader *e = CreateExpiresHeader();
    
    MessageAddHeader(m, (struct Header *)e);
}

void AddContentLengthHeader(struct Message *m)
{
    struct ContentLengthHeader *c = CreateContentLengthHeader();

    MessageAddHeader(m, (struct Header *)c);
}

struct Message *BuildRegisterMessage(char *user, char *host)
{
    struct Message *m = CreateMessage();
    
    AddRequestLine(m, user, host, 5060);
    AddFromHeader(m, user,host,5060);
    AddToHeader(m, user, host, 5060);
    AddViaHeader(m,"192.168.10.63");
    AddContactHeader(m, user, host, 5060);
    AddMaxForwardsHeader(m);
    AddCallIdHeader(m);
    AddCSeqHeader(m);
    AddExpiresHeader(m);
    AddContentLengthHeader(m);
    return m;
}
