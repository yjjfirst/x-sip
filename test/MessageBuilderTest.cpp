#include "CppUTest/TestHarness.h"

extern "C" {
#include "MessageBuilder.h"
#include "Messages.h"
#include "Contacts.h"
#include "ViaHeader.h"
}

char user[] = "Martin Yang";
char host[] = "192.168.10.62";

TEST_GROUP(MessageBuilderTestGroup)
{
    struct Message *m;
    void setup()
    {
        m = BuildRegisterMessage(user, host);
    }

    void teardown()
    {
        DestoryMessage(&m);
    }
};

TEST(MessageBuilderTestGroup, RequestLineTest)
{

    struct RequestLine *rl = MessageGetRequest(m);
    struct URI *uri = RequestLineGetUri(rl);
    
    STRCMP_EQUAL("sip", UriGetScheme(uri));
    STRCMP_EQUAL(host, UriGetHost(uri));
}

TEST(MessageBuilderTestGroup, FromHeaderTest)
{
    struct ContactHeader *from = (struct ContactHeader *) MessageGetHeader("From", m);
    STRCMP_EQUAL("From", ContactsHeaderGetName(from));
    STRCMP_EQUAL("Martin Yang", ContactsHeaderGetDisplayName(from));

    struct URI *uri = ContactsHeaderGetUri(from);
    STRCMP_EQUAL("Martin Yang", UriGetUser(uri));
    STRCMP_EQUAL("192.168.10.62", UriGetHost(uri));
}

TEST(MessageBuilderTestGroup, ToHeaderTest)
{
    struct ContactHeader *to = (struct ContactHeader *) MessageGetHeader("To", m);
    STRCMP_EQUAL("To", ContactsHeaderGetName(to));
    STRCMP_EQUAL("Martin Yang", ContactsHeaderGetDisplayName(to));

    struct URI *uri = ContactsHeaderGetUri(to);
    STRCMP_EQUAL("Martin Yang", UriGetUser(uri));
    STRCMP_EQUAL("192.168.10.62", UriGetHost(uri));
}

TEST(MessageBuilderTestGroup, ViaHeaderTest)
{
    struct ViaHeader *via = (struct ViaHeader *) MessageGetHeader("Via", m);
    
    STRCMP_EQUAL("Via", ViaHeaderGetName(via));
    STRCMP_EQUAL("192.168.10.63", ViaHeaderGetUri(via));
    STRCMP_EQUAL("SIP/2.0/UDP", ViaHeaderGetTransport(via));
}

TEST(MessageBuilderTestGroup, ContactHeaderTest)
{
    struct ContactHeader *contact = (struct ContactHeader *) MessageGetHeader("Contact", m);

    STRCMP_EQUAL("Contact", ContactsHeaderGetName(contact));
}
