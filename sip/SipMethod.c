#include <string.h>
#include <stdio.h>

#include "SipMethod.h"
#include "StringExt.h"
#include "Maps.h"

struct IntStringMap MethodStringMaps[] = {   
    {SIP_METHOD_INVITE,      SIP_METHOD_NAME_INVITE},
    {SIP_METHOD_ACK,         SIP_METHOD_NAME_ACK},
    {SIP_METHOD_OPTIONS,     SIP_METHOD_NAME_OPTIONS},
    {SIP_METHOD_BYE,         SIP_METHOD_NAME_BYE},
    {SIP_METHOD_CANCEL,      SIP_METHOD_NAME_CANCEL},
    {SIP_METHOD_REGISTER,    SIP_METHOD_NAME_REGISTER},
    {SIP_METHOD_INFO,        SIP_METHOD_NAME_INFO},
    {SIP_METHOD_PRACK,       SIP_METHOD_NAME_PRACK},
    {SIP_METHOD_SUBSCRIBE,   SIP_METHOD_NAME_SUBSCRIBE},
    {SIP_METHOD_NOTIFY,      SIP_METHOD_NAME_NOTIFY},
    {SIP_METHOD_UPDATE,      SIP_METHOD_NAME_UPDATE},
    {SIP_METHOD_MESSAGE,     SIP_METHOD_NAME_MESSAGE},
    {SIP_METHOD_REFER,       SIP_METHOD_NAME_REFER},
    {SIP_METHOD_PUBLISH,     SIP_METHOD_NAME_PUBLISH},
    {-1, ""},
};

char *MethodMap2String(SIP_METHOD method)
{
    return IntMap2String(method, MethodStringMaps);
}

SIP_METHOD StringMap2MethodNumber(char *methodString)
{
    int method = String2Int(methodString, MethodStringMaps);

    if (method == -1 ) return SIP_METHOD_NONE;
    else return method;
}

BOOL SipMethodLegal(char *method)
{
    if (StringMap2MethodNumber(method) != SIP_METHOD_NONE)
        return TRUE;
    return FALSE;
}
