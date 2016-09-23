#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int SipUdpInit();
int SipUdpReceiveMessage(char *message, int fd);
int SipUdpSendMessage(char *message, int fd);

extern struct MessageTransporter SipUdpTransporter;
