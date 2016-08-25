
#define SEND_OUT_MESSAGE_MOCK "SendOutMessageMock"
#define RECEIVE_IN_MESSAGE_MOCK "ReceiveInMessageMock"
struct MessageTransporter;
int ReceiveInMessageMock(char *message);
int SendOutMessageMock(char *message);

extern struct MessageTransporter MockTransporter;
extern struct MessageTransporter DummyTransporter;
