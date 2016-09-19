#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "Transporter.h"
#include "Udp.h"

#define MAXLINE 80
#define SERV_PORT 5060

static int sockfd = -1;

int (*xbind)(int sockfd, const struct sockaddr *addr,
         socklen_t addrlen) = bind;

int (*xsocket)(int domain, int type, int protocol) = socket;    

ssize_t (*xsendto)(int sockfd, const void *buf, size_t len, int flags,
                   const struct sockaddr *dest_addr, socklen_t addrlen) = sendto;

ssize_t (*xrecvfrom)(int sockfd, void *buf, size_t len, int flags,
                    struct sockaddr *src_addr, socklen_t *addrlen) = recvfrom;

int UdpReceiveMessage(char *message)
{
    struct sockaddr_in from_addr;
    socklen_t len = sizeof(from_addr);

    from_addr.sin_family = AF_INET;
    from_addr.sin_port = htons(5060);
    inet_aton("192.168.10.62", &from_addr.sin_addr);

    xrecvfrom(sockfd, message, 2048, 0, (struct sockaddr *)&from_addr, &len); 

    return 0;
}

int UdpSendMessage(char *message)
{
    struct sockaddr_in dest_addr;
    int slen = sizeof(dest_addr);

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(5060);
    inet_aton("192.168.10.62", &dest_addr.sin_addr);
    
    xsendto(sockfd, message, strlen(message), 0, (void*)&dest_addr, slen);

    return 0;
}

int UdpInit()
{
    struct sockaddr_in servaddr;

    if ( (sockfd = xsocket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        return -1;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    if(xbind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
        return -1;
    }

    return sockfd;
}

struct MessageTransporter UdpTransporter = {
    .send = UdpSendMessage,
    .receive = UdpReceiveMessage,
    .init = UdpInit,
};
