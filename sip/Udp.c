#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "Transporter.h"

#define MAXLINE 80
#define SERV_PORT 5060

static int sockfd;

int UdpSendMessage(char *message)
{
    struct sockaddr_in dest_addr;
    int slen = sizeof(dest_addr);
    
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(5060);
    inet_aton("192.168.10.62", &dest_addr.sin_addr);
    
    return sendto(sockfd, message, strlen(message), 0, (void*)&dest_addr, slen);
}

int UdpReceiveMessage(char *message)
{
    return 0;
}

struct MessageTransporter UdpTransporter = {
    .send = UdpSendMessage,
    .receive = UdpReceiveMessage,
};

void UdpTransporterInit()
{
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    if(bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
        perror("bind error");
        exit(1);
    }
}
