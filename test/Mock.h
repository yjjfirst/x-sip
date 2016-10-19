#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SEND_OUT_MESSAGE_MOCK "SendOutMessageMock"
#define RECEIVE_IN_MESSAGE_MOCK "ReceiveInMessageMock"
struct MessageTransporter;
int ReceiveMessageMock(char *message, int fd);
int SendMessageMock(char *message, char *destaddr, int destport, int fd);

void GenerateBranchMock(char *branch);
void GenerateBranchMockForAuthorization(char *branch);
    
extern struct MessageTransporter MockTransporter;
extern struct MessageTransporter DummyTransporter;
extern struct MessageTransporter MockTransporterAndHandle;
extern struct MessageTransporter ClientTransporterMock;

void NotifyCallManagerMock(int event, struct UserAgent *ua);

int bind_mock(int sockfd, const struct sockaddr *addr,
              socklen_t addrlen);
int socket_mock(int domain, int type, int protocol);

ssize_t sendto_mock(int sockfd, const void *buf, size_t len, int flags,
                    const struct sockaddr *dest_addr, socklen_t addrlen);
ssize_t recvfrom_mock(int sockfd, void *buf, size_t len, int flags,
                      struct sockaddr *src_addr, socklen_t *addrlen);
