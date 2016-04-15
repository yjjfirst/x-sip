#include "CppUTest/TestHarness.h"

extern "C" {
#include <string.h>
#include "SipMethod.h"
}

TEST_GROUP(SipMethodGroup)
{
};

TEST(SipMethodGroup, MethodMap2StringTest)
{
    SIP_METHOD method = SIP_METHOD_REGISTER;    
    STRCMP_EQUAL(SIP_METHOD_NAME_REGISTER, MethodMap2String(method));

    method = SIP_METHOD_INVITE;
    STRCMP_EQUAL(SIP_METHOD_NAME_INVITE, MethodMap2String(method));

    method = SIP_METHOD_INFO;
    STRCMP_EQUAL(SIP_METHOD_NAME_INFO, MethodMap2String(method));
    
    method = SIP_METHOD_PUBLISH;
    STRCMP_EQUAL(SIP_METHOD_NAME_PUBLISH, MethodMap2String(method));
}

TEST(SipMethodGroup, StringMap2MethodNumber)
{
    char methodString[64] = SIP_METHOD_NAME_INVITE;
    CHECK_EQUAL(SIP_METHOD_INVITE, StringMap2MethodNumber(methodString));
    
    strcpy(methodString, SIP_METHOD_NAME_REGISTER);
    CHECK_EQUAL(SIP_METHOD_REGISTER, StringMap2MethodNumber(methodString));

    strcpy(methodString, SIP_METHOD_NAME_PUBLISH);
    CHECK_EQUAL(SIP_METHOD_PUBLISH, StringMap2MethodNumber(methodString));
    
    strcpy(methodString, "ASDFADF");
    CHECK_EQUAL(SIP_METHOD_NONE, StringMap2MethodNumber(methodString));

}

IGNORE_TEST(SipMethodGroup, MethodLegalTest)
{
    FAIL("");
}
