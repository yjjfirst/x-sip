#include "Provision.h"

char *GetUserNameImpl(void)
{
    return "88001";
}

char *(*GetUserName)(void) = GetUserNameImpl;

char *GetProxyImpl(void)
{
    return "192.168.10.62";
}
char *(*GetProxy)(void) = GetProxyImpl;

char *GetRegistrarImpl(void)
{
    return "192.168.10.62";
}
char *(*GetRegistrar)(void) = GetRegistrarImpl;


char *GetLocalIpAddrImpl(void)
{
    return "192.168.10.63";
}
char *(*GetLocalIpAddr)(void) = GetLocalIpAddrImpl;
