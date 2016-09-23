#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int ClientInit();
int ClientReceiveMessage(char *message, int fd);
int ClientSendMessage(char *message, int fd);

extern struct MessageTransporter ClientTransporter;
