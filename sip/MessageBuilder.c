#include "Messages.h"
#include "Contacts.h"

struct Message *BuildRegisterMessage(char *user, char *host)
{

    struct URI *uri = CreateUri();
    struct RequestLine  *r = CreateRequestLine(REGISTER, NULL);
    struct Message *m = CreateMessage();
    struct ContactHeader *from = CreateContactsHeader();
     
    UriSetUser(uri, user);
    UriSetHost(uri, host);
    RequestLineSetUri(r, uri);

    ContactsHeaderSetName(from, "From");
    ContactsHeaderSetDisplayName(from, "Martin Yang");
    MessageAddHeader(m, (struct Header *)from);

    MessageSetRequest(m, r);
    
    return m;
}
