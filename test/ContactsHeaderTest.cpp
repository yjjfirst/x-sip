#include "CppUTest/TestHarness.h"

extern "C" {
#include "Contacts.h"
#include "Parser.h"
}

TEST_GROUP(ToTestGroup)
{
};

TEST(ToTestGroup, ContactHeaderParseTest)
{
    struct ContactHeader *toHeader = CreateContactsHeader();

    char toString[]= "To:Martin Yang<sip:Martin.Yang@cs.columbia.edu>;tag=287447";

    Parse((char *)toString, toHeader, GetContactsHeaderPattern(toString));    
    STRCMP_EQUAL("To", ContactsHeaderGetName(toHeader));
    STRCMP_EQUAL("Martin Yang", ContactsHeaderGetDisplayName(toHeader));
    STRCMP_EQUAL("sip:Martin.Yang@cs.columbia.edu", ContactsHeaderGetUri(toHeader));
    STRCMP_EQUAL("tag=287447", ContactsHeaderGetParameters(toHeader));
    
    DestoryContactsHeader((struct Header *)toHeader);
}

TEST(ToTestGroup, ContactHeaderWithSpaceParseTest)
{
    struct ContactHeader *toHeader = CreateContactsHeader();
    char toString[]= "To:  Martin Yang  <sip:Martin.Yang@cs.columbia.edu>  ;  tag=287447";

    Parse((char *)toString, toHeader, GetContactsHeaderPattern(toString));    
    STRCMP_EQUAL("To", ContactsHeaderGetName(toHeader));
    STRCMP_EQUAL("Martin Yang", ContactsHeaderGetDisplayName(toHeader));
    STRCMP_EQUAL("sip:Martin.Yang@cs.columbia.edu", ContactsHeaderGetUri(toHeader));
    STRCMP_EQUAL("tag=287447", ContactsHeaderGetParameters(toHeader));
    
    DestoryContactsHeader((struct Header *)toHeader);
}

TEST(ToTestGroup, ContactHeaderQuotedDisplayNameParseTest)
{
    struct ContactHeader *toHeader = CreateContactsHeader();
    char toString[]= "To:  \"Martin Yang\"  <sip:Martin.Yang@cs.columbia.edu>  ;  tag=287447";

    Parse((char *)toString, toHeader, GetContactsHeaderPattern(toString));    
    STRCMP_EQUAL("To", ContactsHeaderGetName(toHeader));
    STRCMP_EQUAL("Martin Yang", ContactsHeaderGetDisplayName(toHeader));
    STRCMP_EQUAL("sip:Martin.Yang@cs.columbia.edu", ContactsHeaderGetUri(toHeader));
    STRCMP_EQUAL("tag=287447", ContactsHeaderGetParameters(toHeader));
    
    DestoryContactsHeader((struct Header *)toHeader);
}

TEST(ToTestGroup, ContactHeaderNoDisplayNameParseTest)
{
    struct ContactHeader *toHeader = CreateContactsHeader();
    char toString[]= "To:sip:Martin.Yang@cs.columbia.edu;tag=287447";

    Parse((char *)toString, toHeader, GetContactsHeaderPattern(toString));    
    STRCMP_EQUAL("To", ContactsHeaderGetName(toHeader));
    STRCMP_EQUAL("", ContactsHeaderGetDisplayName(toHeader));
    STRCMP_EQUAL("sip:Martin.Yang@cs.columbia.edu", ContactsHeaderGetUri(toHeader));
    STRCMP_EQUAL("tag=287447", ContactsHeaderGetParameters(toHeader));
    
    DestoryContactsHeader((struct Header *)toHeader);
    
}

TEST(ToTestGroup, ContactParseTest)
{
    struct ContactHeader *contact = CreateContactsHeader();
    char contactString[] = "Contact:<sip:alice@pc33.atlanta.com>";
   
    Parse((char*) contactString, contact, GetContactsHeaderPattern(contactString));
    STRCMP_EQUAL("Contact", ContactsHeaderGetName(contact));
    STRCMP_EQUAL("sip:alice@pc33.atlanta.com", ContactsHeaderGetUri(contact));
    DestoryContactsHeader((struct Header *)contact);
}

TEST(ToTestGroup, Contact2StringTest)
{
    struct ContactHeader *contact = CreateContactsHeader();
    char contactString[] = "Contact:<sip:alice@pc33.atlanta.com>";
    char result[128] = {0};

    Parse((char*) contactString, contact, GetContactsHeaderPattern(contactString));
    ContactsHeader2String(result, contact);
    STRCMP_EQUAL("Contact:<sip:alice@pc33.atlanta.com>", result);

    DestoryContactsHeader((struct Header *)contact);
}

TEST(ToTestGroup, ContactWithQuotedDisplayName2StringTest)
{
    struct ContactHeader *toHeader = CreateContactsHeader();
    char toString[]= "To:\"Martin Yang\"<sip:Martin.Yang@cs.columbia.edu>;tag=287447";
    char result[128] = {0};

    Parse((char *)toString, toHeader, GetContactsHeaderPattern(toString));    
    ContactsHeader2String(result, toHeader);
    STRCMP_EQUAL(toString, result);
    
    DestoryContactsHeader((struct Header *)toHeader);
}
