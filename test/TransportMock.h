struct MessageTransporter;
int ReceiveInMessageMock(char *message);
int SendOutMessageMock(char *message);

extern MessageTransporter MockTransporter;
