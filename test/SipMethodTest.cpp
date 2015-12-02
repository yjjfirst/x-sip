#include "CppUTest/TestHarness.h"

extern "C" {
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
