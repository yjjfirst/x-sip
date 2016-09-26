#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int ClientInit(int port);
int ClientReceiveMessage(char *message, int fd);
int ClientSendMessage(char *message, char *destaddr, int destport, int fd);

extern struct MessageTransporter ClientTransporter;
