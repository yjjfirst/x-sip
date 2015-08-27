#include "CppUTest/TestHarness.h"

extern "C" {
#include "Contacts.h"
#include "strings.h"
}

TEST_GROUP(ToTestGroup)
{
};

TEST(ToTestGroup, ToHeaderParseTest)
{
    struct ToHeader *toHeader = CreateContactsHeader();

    char toString[]= "To:Martin Yang<sip:Martin.Yang@cs.columbia.edu>;tag=287447";

    Parse((char *)toString, toHeader, GetContactsHeaderPattern(toString));    
    STRCMP_EQUAL("To", ContactsHeaderGetName(toHeader));
    STRCMP_EQUAL("Martin Yang", ContactsHeaderGetDisplayName(toHeader));
    STRCMP_EQUAL("sip:Martin.Yang@cs.columbia.edu", ContactsHeaderGetUri(toHeader));
    STRCMP_EQUAL("tag=287447", ContactsHeaderGetParameters(toHeader));
    
    DestoryContactsHeader(toHeader);
}

TEST(ToTestGroup, ToHeaderWithSpaceParseTest)
{
    struct ToHeader *toHeader = CreateContactsHeader();
    char toString[]= "To:  Martin Yang  <sip:Martin.Yang@cs.columbia.edu>  ;  tag=287447";

    Parse((char *)toString, toHeader, GetContactsHeaderPattern(toString));    
    STRCMP_EQUAL("To", ContactsHeaderGetName(toHeader));
    STRCMP_EQUAL("Martin Yang", ContactsHeaderGetDisplayName(toHeader));
    STRCMP_EQUAL("sip:Martin.Yang@cs.columbia.edu", ContactsHeaderGetUri(toHeader));
    STRCMP_EQUAL("tag=287447", ContactsHeaderGetParameters(toHeader));
    
    DestoryContactsHeader(toHeader);
}

TEST(ToTestGroup, ToHeaderQuotedDisplayNameParseTest)
{
    struct ToHeader *toHeader = CreateContactsHeader();
    char toString[]= "To:  \"Martin Yang\"  <sip:Martin.Yang@cs.columbia.edu>  ;  tag=287447";

    Parse((char *)toString, toHeader, GetContactsHeaderPattern(toString));    
    STRCMP_EQUAL("To", ContactsHeaderGetName(toHeader));
    STRCMP_EQUAL("Martin Yang", ContactsHeaderGetDisplayName(toHeader));
    STRCMP_EQUAL("sip:Martin.Yang@cs.columbia.edu", ContactsHeaderGetUri(toHeader));
    STRCMP_EQUAL("tag=287447", ContactsHeaderGetParameters(toHeader));
    
    DestoryContactsHeader(toHeader);
}

TEST(ToTestGroup, ToHeaderNoDisplayNameParseTest)
{
    struct ToHeader *toHeader = CreateContactsHeader();
    char toString[]= "To:sip:Martin.Yang@cs.columbia.edu;tag=287447";

    Parse((char *)toString, toHeader, GetContactsHeaderPattern(toString));    
    STRCMP_EQUAL("To", ContactsHeaderGetName(toHeader));
    STRCMP_EQUAL("", ContactsHeaderGetDisplayName(toHeader));
    STRCMP_EQUAL("sip:Martin.Yang@cs.columbia.edu", ContactsHeaderGetUri(toHeader));
    STRCMP_EQUAL("tag=287447", ContactsHeaderGetParameters(toHeader));
    
    DestoryContactsHeader(toHeader);
    
}

TEST(ToTestGroup, ContactParseTest)
{
    struct ToHeader *contact = CreateContactsHeader();
    char contactString[] = "Contact:<sip:alice@pc33.atlanta.com>";
   
    Parse((char*) contactString, contact, GetContactsHeaderPattern(contactString));
    STRCMP_EQUAL("Contact", ContactsHeaderGetName(contact));
    STRCMP_EQUAL("sip:alice@pc33.atlanta.com", ContactsHeaderGetUri(contact));
    DestoryContactsHeader(contact);
}
