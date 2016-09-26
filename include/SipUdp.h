#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int SipUdpInit(int port);
int SipUdpReceiveMessage(char *message, int fd);
int SipUdpSendMessage(char *message, char *destaddr, int destport, int fd);

extern struct MessageTransporter SipUdpTransporter;
