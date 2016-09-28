#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "Udp.h"

int (*xbind)(int sockfd, const struct sockaddr *addr,
         socklen_t addrlen) = bind;

int (*xsocket)(int domain, int type, int protocol) = socket;    

ssize_t (*xsendto)(int sockfd, const void *buf, size_t len, int flags,
                   const struct sockaddr *dest_addr, socklen_t addrlen) = sendto;

ssize_t (*xrecvfrom)(int sockfd, void *buf, size_t len, int flags,
                    struct sockaddr *src_addr, socklen_t *addrlen) = recvfrom;

int UdpReceive(char *message, int fd)
{
    struct sockaddr_in from_addr;
    socklen_t len = sizeof(from_addr);

    from_addr.sin_family = AF_INET;
    from_addr.sin_port = htons(0);
    inet_aton("0.0.0.0", &from_addr.sin_addr);

    xrecvfrom(fd, message, 2048, 0, (struct sockaddr *)&from_addr, &len);

    return 0;
}

int UdpSendImpl(char *message, char *destaddr, int port, int fd)
{
    struct sockaddr_in dest_addr;
    int slen = sizeof(dest_addr);

    assert(destaddr != NULL);
    
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    inet_aton(destaddr, &dest_addr.sin_addr);
    
    xsendto(fd, message, strlen(message), 0, (void*)&dest_addr, slen);

    return 0;
}

int (*UdpSend)(char *message, char *destaddr, int port,  int ft) = UdpSendImpl;

int UdpInit(int port)
{
    struct sockaddr_in servaddr;
    int sockfd = -1;
    
    if ( (sockfd = xsocket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        return -1;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_aton("0.0.0.0", &servaddr.sin_addr);
    servaddr.sin_port = htons(port);

    if(xbind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
        return -1;
    }

    return sockfd;
}
