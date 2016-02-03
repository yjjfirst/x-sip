#include "Provision.h"

char *GetUserNameImpl(int i)
{
    return "88001";
}
char *(*GetUserName)(int i) = GetUserNameImpl;

char *GetProxyImpl(int i)
{
    return "192.168.10.62";
}
char *(*GetProxy)(int i) = GetProxyImpl;

char *GetRegistrarImpl(int i)
{
    return "192.168.10.62";
}
char *(*GetRegistrar)(int i) = GetRegistrarImpl;


char *GetLocalIpAddrImpl(void)
{
    return "192.168.10.63";
}
char *(*GetLocalIpAddr)(void) = GetLocalIpAddrImpl;
