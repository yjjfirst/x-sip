#include "Messages.h"
#include "Contacts.h"

struct Message *BuildRegisterMessage(char *user, char *host)
{

    struct URI *uri = CreateUri();
    struct RequestLine  *r = CreateRequestLine(REGISTER, NULL);
    struct Message *m = CreateMessage();
    struct ContactHeader *from = CreateContactsHeader();
    struct URI *fromUri = NULL;
    //struct URI *toUri = NULL;
     
    UriSetScheme(uri, "sip");
    UriSetHost(uri, host);
    RequestLineSetUri(r, uri);
    MessageSetRequest(m, r);

    ContactsHeaderSetName(from, "From");
    ContactsHeaderSetDisplayName(from, "Martin Yang");
    fromUri = UriDup(uri);
    ContactsHeaderSetUri(from, fromUri);
    MessageAddHeader(m, (struct Header *)from);

 
    return m;
}
