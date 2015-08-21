#include "CppUTest/TestHarness.h"

extern "C" {
#include "ToHeader.h"
#include "strings.h"
}

TEST_GROUP(ToTestGroup)
{
};

TEST(ToTestGroup, ToHeaderParseTest)
{
    struct ToHeader *toHeader = CreateToHeader();

    char toString[]= "To:Martin Yang<sip:Martin.Yang@cs.columbia.edu>;tag=287447";

    Parse((char *)toString, toHeader, GetToHeaderPattern(toString));    
    STRCMP_EQUAL("To", toHeader->name);
    STRCMP_EQUAL("Martin Yang", toHeader->displayName);
    STRCMP_EQUAL("sip:Martin.Yang@cs.columbia.edu", toHeader->uri);
    STRCMP_EQUAL("tag=287447", toHeader->parameters);
    
    DestoryToHeader(toHeader);
}

TEST(ToTestGroup, ToHeaderWithSpaceParseTest)
{
    struct ToHeader *toHeader = CreateToHeader();
    char toString[]= "To:  Martin Yang  <sip:Martin.Yang@cs.columbia.edu>  ;  tag=287447";

    Parse((char *)toString, toHeader, GetToHeaderPattern(toString));    
    STRCMP_EQUAL("To", toHeader->name);
    STRCMP_EQUAL("Martin Yang", toHeader->displayName);
    STRCMP_EQUAL("sip:Martin.Yang@cs.columbia.edu", toHeader->uri);
    STRCMP_EQUAL("tag=287447", toHeader->parameters);
    
    DestoryToHeader(toHeader);
}

TEST(ToTestGroup, ToHeaderQuotedDisplayNameParseTest)
{
    struct ToHeader *toHeader = CreateToHeader();
    char toString[]= "To:  \"Martin Yang\"  <sip:Martin.Yang@cs.columbia.edu>  ;  tag=287447";

    Parse((char *)toString, toHeader, GetToHeaderPattern(toString));    
    STRCMP_EQUAL("To", toHeader->name);
    STRCMP_EQUAL("Martin Yang", toHeader->displayName);
    STRCMP_EQUAL("sip:Martin.Yang@cs.columbia.edu", toHeader->uri);
    STRCMP_EQUAL("tag=287447", toHeader->parameters);
    
    DestoryToHeader(toHeader);
}

TEST(ToTestGroup, ToHeaderNoDisplayNameParseTest)
{
    struct ToHeader *toHeader = CreateToHeader();
    char toString[]= "To:sip:Martin.Yang@cs.columbia.edu;tag=287447";

    Parse((char *)toString, toHeader, GetToHeaderPattern(toString));    
    STRCMP_EQUAL("To", toHeader->name);
    STRCMP_EQUAL("", toHeader->displayName);
    STRCMP_EQUAL("sip:Martin.Yang@cs.columbia.edu", toHeader->uri);
    STRCMP_EQUAL("tag=287447", toHeader->parameters);
    
    DestoryToHeader(toHeader);
    
}
