#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int UdpTransporterInit();
int UdpReceiveMessage(char *message);
int UdpSendMessage(char *message);

extern struct MessageTransporter UdpTransporter;

extern int (*xbind)(int sockfd, const struct sockaddr *addr,
         socklen_t addrlen);
extern int (*xsocket)(int domain, int type, int protocol);
extern ssize_t (*xsendto)(int sockfd, const void *buf, size_t len, int flags,
                   const struct sockaddr *dest_addr, socklen_t addrlen);
extern ssize_t (*xrecvfrom)(int sockfd, void *buf, size_t len, int flags,
                            struct sockaddr *src_addr, socklen_t *addrlen);
