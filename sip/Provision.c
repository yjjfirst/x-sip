#include "Provision.h"

char *GetUserNameImpl(int i)
{
    return "88001";
}

char *GetProxyImpl(int i)
{
    return "192.168.10.62";
}

char *GetRegistrarImpl(int i)
{
    return "192.168.10.62";
}

char *GetLocalIpAddrImpl(void)
{
    return "192.168.10.1";
}

char *(*GetUserName)(int i) = GetUserNameImpl;
char *(*GetRegistrar)(int i) = GetRegistrarImpl;
char *(*GetProxy)(int i) = GetProxyImpl;
char *(*GetLocalIpAddr)(void) = GetLocalIpAddrImpl;
