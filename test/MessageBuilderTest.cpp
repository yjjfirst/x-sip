#include "CppUTest/TestHarness.h"

extern "C" {
#include "MessageBuilder.h"
#include "Messages.h"
#include "Contacts.h"
}

TEST_GROUP(MessageBuilderTestGroup)
{

};

TEST(MessageBuilderTestGroup, BuildRegisterMessageTest)
{
    char user[] = "Martin Yang";
    char host[] = "192.168.10.62";

    struct Message *m = BuildRegisterMessage(user, host);
    struct RequestLine *rl = MessageGetRequest(m);
    struct URI *uri = RequestLineGetUri(rl);
    
    STRCMP_EQUAL("Martin Yang", UriGetUser(uri));
    STRCMP_EQUAL("192.168.10.62", UriGetHost(uri));

    struct ContactHeader *from = (struct ContactHeader *) MessageGetHeader("FROM", m);
    STRCMP_EQUAL("From", ContactsHeaderGetName(from));
    STRCMP_EQUAL("Martin Yang", ContactsHeaderGetDisplayName(from));

    DestoryMessage(&m);
}


















