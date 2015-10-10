#include "Messages.h"
#include "Contacts.h"
#include "ViaHeader.h"

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
    struct ContactHeader *from = CreateContactsHeader();

    ContactsHeaderSetName(from, "From");
    ContactsHeaderSetDisplayName(from, "Martin Yang");
    ContactsHeaderSetUri(from, uri);
    MessageAddHeader(m, (struct Header *)from);
}

void AddToHeader(struct Message *m, char *user, char *host, int port)
{
    struct URI *uri = CreateUri("sip", user, host, port);
    struct ContactHeader *to = CreateContactsHeader();

    ContactsHeaderSetName(to, "To");
    ContactsHeaderSetDisplayName(to, "Martin Yang");
    ContactsHeaderSetUri(to, uri);
    MessageAddHeader(m, (struct Header *)to);
}

void AddContactHeader(struct Message *m, char *usr, char *host, int port)
{
    struct URI *uri = CreateUri("sip", usr, host, port);
    struct ContactHeader *contact = CreateContactsHeader();

    ContactsHeaderSetName(contact, "Contact");
    ContactsHeaderSetUri(contact, uri);
    MessageAddHeader(m, (struct Header *)contact);
}

struct Message *BuildRegisterMessage(char *user, char *host)
{
    struct Message *m = CreateMessage();
    
    AddRequestLine(m, user, host, 5060);
    AddFromHeader(m, user,host,5060);
    AddToHeader(m, user, host, 5060);
    AddViaHeader(m,"192.168.10.63");
    AddContactHeader(m, user, host, 5060);
 
    return m;
}
