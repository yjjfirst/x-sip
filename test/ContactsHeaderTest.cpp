#include "CppUTest/TestHarness.h"

extern "C" {
#include <stdio.h>
#include <string.h>

#include "URI.h"
#include "ContactHeader.h"
#include "Parser.h"
#include "Header.h"
#include "Parameter.h"
}

TEST_GROUP(ContactHeaderTestGroup)
{
    void UriCheck(struct ContactHeader *header)
    {
        struct URI *uri = ContactHeaderGetUri(header);
        STRCMP_EQUAL("sip", UriGetScheme(uri));
        STRCMP_EQUAL("Martin.Yang", UriGetUser(uri));
        STRCMP_EQUAL("cs.columbia.edu",UriGetHost(uri));
    }
};

TEST(ContactHeaderTestGroup, ContactHeaderParseTest)
{
    struct ContactHeader *toHeader = CreateContactHeader();

    char toString[]= "To:Martin Yang<sip:Martin.Yang@cs.columbia.edu>;tag=287447";

    Parse((char *)toString, toHeader, GetContactHeaderPattern(toString));    
    STRCMP_EQUAL("To", ContactHeaderGetName(toHeader));
    STRCMP_EQUAL("Martin Yang", ContactHeaderGetDisplayName(toHeader));

    UriCheck(toHeader);

    STRCMP_EQUAL("287447", ContactHeaderGetParameter(toHeader, HEADER_PARAMETER_NAME_TAG));
    
    DestroyContactHeader((struct Header *)toHeader);
}

TEST(ContactHeaderTestGroup, ContactHeaderWithSpaceParseTest)
{
    struct ContactHeader *toHeader = CreateContactHeader();
    char toString[]= "To:  Martin Yang  <sip:Martin.Yang@cs.columbia.edu>  ;  tag=287447";

    Parse((char *)toString, toHeader, GetContactHeaderPattern(toString));    
    STRCMP_EQUAL("To", ContactHeaderGetName(toHeader));
    STRCMP_EQUAL("Martin Yang", ContactHeaderGetDisplayName(toHeader));

    UriCheck(toHeader);

    STRCMP_EQUAL("287447", ContactHeaderGetParameter(toHeader, HEADER_PARAMETER_NAME_TAG));
    
    DestroyContactHeader((struct Header *)toHeader);
}

TEST(ContactHeaderTestGroup, ContactHeaderQuotedDisplayNameParseTest)
{
    struct ContactHeader *toHeader = CreateContactHeader();
    char toString[]= "To:  \"Martin Yang\"  <sip:Martin.Yang@cs.columbia.edu>  ;  tag=287447";

    Parse((char *)toString, toHeader, GetContactHeaderPattern(toString));    
    STRCMP_EQUAL("To", ContactHeaderGetName(toHeader));
    STRCMP_EQUAL("Martin Yang", ContactHeaderGetDisplayName(toHeader));

    UriCheck(toHeader);

    STRCMP_EQUAL("287447", ContactHeaderGetParameter(toHeader, HEADER_PARAMETER_NAME_TAG));
    
    DestroyContactHeader((struct Header *)toHeader);
}

TEST(ContactHeaderTestGroup, ContactHeaderNoDisplayNameParseTest)
{
    struct ContactHeader *toHeader = CreateContactHeader();
    char toString[]= "To:sip:Martin.Yang@cs.columbia.edu;tag=287447";

    Parse((char *)toString, toHeader, GetContactHeaderPattern(toString));    
    STRCMP_EQUAL("To", ContactHeaderGetName(toHeader));
    STRCMP_EQUAL("", ContactHeaderGetDisplayName(toHeader));

    UriCheck(toHeader);

    STRCMP_EQUAL("287447", ContactHeaderGetParameter(toHeader, HEADER_PARAMETER_NAME_TAG));
    
    DestroyContactHeader((struct Header *)toHeader);
    
}

TEST(ContactHeaderTestGroup, ContactParseTest)
{
    struct ContactHeader *contact = CreateContactHeader();
    char contactString[] = "Contact:<sip:Martin.Yang@cs.columbia.edu>";
   
    Parse((char*) contactString, contact, GetContactHeaderPattern(contactString));
    STRCMP_EQUAL("Contact", ContactHeaderGetName(contact));
    UriCheck(contact);

    DestroyContactHeader((struct Header *)contact);
}

TEST(ContactHeaderTestGroup, Contact2StringTest)
{
    struct ContactHeader *contact = CreateContactHeader();
    char contactString[] = "Contact:<sip:alice@pc33.atlanta.com>";
    char result[128] = {0};

    Parse((char*) contactString, contact, GetContactHeaderPattern(contactString));
    ContactHeader2String(result,(struct Header *) contact);
    STRCMP_EQUAL("Contact:<sip:alice@pc33.atlanta.com>", result);

    DestroyContactHeader((struct Header *)contact);
}

TEST(ContactHeaderTestGroup, ContactWithQuotedDisplayName2StringTest)
{
    struct ContactHeader *toHeader = CreateContactHeader();
    char toString[]= "To:\"Martin Yang\"<sip:Martin.Yang@cs.columbia.edu>;tag=287447";
    char result[128] = {0};

    Parse((char *)toString, toHeader, GetContactHeaderPattern(toString));    
    ContactHeader2String(result, (struct Header *)toHeader);
    STRCMP_EQUAL(toString, result);
    
    DestroyContactHeader((struct Header *)toHeader);
}

TEST(ContactHeaderTestGroup, ContactStartWithSpaceParseTest)
{
    struct ContactHeader *toHeader = CreateContactHeader();
    char contactString[] = "Contact: <sip:bob@192.0.2.4>";

    Parse((char *)contactString, toHeader, GetContactHeaderPattern(contactString));
    DestroyContactHeader((struct Header *)toHeader);    
}

TEST(ContactHeaderTestGroup, CreateToHeaderTest)
{
    struct ContactHeader *toHeader = CreateToHeader();

    STRCMP_EQUAL("To", ContactHeaderGetName(toHeader));

    DestroyContactHeader( (struct Header *)toHeader);
}

TEST(ContactHeaderTestGroup, CreateFromHeaderTest)
{
    struct ContactHeader *from = CreateFromHeader();

    STRCMP_EQUAL("From", ContactHeaderGetName(from));

    DestroyContactHeader( (struct Header *)from);

}

TEST(ContactHeaderTestGroup, CreateContactHeaderTest)
{
    struct ContactHeader *c = CreateContactHeader();

    STRCMP_EQUAL("Contact", ContactHeaderGetName(c));

    DestroyContactHeader( (struct Header *)c);
}

TEST(ContactHeaderTestGroup, ContactHeaderMatchedTest)
{
    struct ContactHeader *header1 = CreateContactHeader();
    struct ContactHeader *header2 = CreateContactHeader();
    char toString[]= "To:sip:Martin.Yang@cs.columbia.edu;tag=287447";

    Parse((char *)toString, header1, GetContactHeaderPattern(toString));    
    Parse((char *)toString, header2, GetContactHeaderPattern(toString));
    CHECK_TRUE(ContactHeaderMatched(header1, header2));
    
    DestroyContactHeader((struct Header *)header1);
    DestroyContactHeader((struct Header *)header2);
}

TEST(ContactHeaderTestGroup, ContactHeaderDisplayNameUnmatchedTest)
{
    struct ContactHeader *header1 = CreateContactHeader();
    struct ContactHeader *header2 = CreateContactHeader();
    char toString1[]= "To:\"Peter Zhang\"<sip:Martin.Yang@cs.columbia.edu>;tag=287447";
    char toString2[]= "To:\"Martin Yang\"<sip:Martin.Yang@cs.columbia.edu>;tag=287447";

    Parse((char *)toString1, header1, GetContactHeaderPattern(toString1));    
    Parse((char *)toString2, header2, GetContactHeaderPattern(toString2));
    CHECK_FALSE(ContactHeaderMatched(header1, header2));
    
    DestroyContactHeader((struct Header *)header1);
    DestroyContactHeader((struct Header *)header2);
}

TEST(ContactHeaderTestGroup, ContactHeaderUriUnmatchedTest)
{
    struct ContactHeader *header1 = CreateContactHeader();
    struct ContactHeader *header2 = CreateContactHeader();
    char toString1[]= "To:\"Martin Yang\"<sip:Martin.Yang@cs.columbia.edu>;tag=287447";
    char toString2[]= "To:\"Martin Yang\"<sips:Martin.Yang@cs.columbia.edu>;tag=287447";

    Parse((char *)toString1, header1, GetContactHeaderPattern(toString1));    
    Parse((char *)toString2, header2, GetContactHeaderPattern(toString2));
    CHECK_FALSE(ContactHeaderMatched(header1, header2));
    
    DestroyContactHeader((struct Header *)header1);
    DestroyContactHeader((struct Header *)header2);
}

TEST(ContactHeaderTestGroup, ContactHeaderParametersUnmatchedTest)
{
    struct ContactHeader *header1 = CreateContactHeader();
    struct ContactHeader *header2 = CreateContactHeader();
    char toString1[]= "To:\"Martin Yang\"<sip:Martin.Yang@cs.columbia.edu>;tag=287447";
    char toString2[]= "To:\"Martin Yang\"<sip:Martin.Yang@cs.columbia.edu>;tag=287448";

    Parse((char *)toString1, header1, GetContactHeaderPattern(toString1));    
    Parse((char *)toString2, header2, GetContactHeaderPattern(toString2));
    CHECK_FALSE(ContactHeaderMatched(header1, header2));
    
    DestroyContactHeader((struct Header *)header1);
    DestroyContactHeader((struct Header *)header2);
}

TEST(ContactHeaderTestGroup, ContactHeaderDupTest)
{
    struct ContactHeader *header = CreateContactHeader();
    char toString[] = "To:\"Martin Yang\"<sip:Martin.Yang@cs.columbia.edu>;tag=287447";
    Parse(toString, header, GetContactHeaderPattern(toString));
    
    struct ContactHeader *headerDup = ContactHeaderDup(header);
    CHECK_TRUE(ContactHeaderMatched(header, headerDup));

    DestroyContactHeader((struct Header *)headerDup);
    DestroyContactHeader((struct Header *)header);
}

TEST(ContactHeaderTestGroup, ContactHeaderRemoveParametersTest)
{
    struct ContactHeader *header = CreateContactHeader();
    char toString[] = "To:\"Martin Yang\"<sip:Martin.Yang@cs.columbia.edu>;tag=287447";
    Parse(toString, header, GetContactHeaderPattern(toString));

    ContactHeaderRemoveParameters(header);
    struct Parameters *ps = ContactHeaderGetParameters(header);

    CHECK_EQUAL(0, ParametersLength(ps));
    
    DestroyContactHeader((struct Header *)header);
}

void GenerateTagMock(char *tag)
{
    strcpy(tag, "abcdefghijk");
}

TEST(ContactHeaderTestGroup, GenerateTagTest)
{
    char tag[MAX_TAG_LENGTH] = {0};

    UT_PTR_SET(GenerateTag, GenerateTagMock);
    GenerateTag(tag);
    
    STRCMP_EQUAL("abcdefghijk", tag);
}
