int ParseSdpImpl(char *content, unsigned int length)
{
    return 0;
}

int (*ParseSdp)(char *content, unsigned int length) = ParseSdpImpl;
