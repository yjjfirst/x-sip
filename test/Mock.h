
#define SEND_OUT_MESSAGE_MOCK "SendOutMessageMock"
#define RECEIVE_IN_MESSAGE_MOCK "ReceiveInMessageMock"
struct MessageTransporter;
int ReceiveInMessageMock(char *message);
int SendOutMessageMock(char *message);

void GenerateBranchMock(char *branch);
void GenerateBranchMockForAuthorization(char *branch);
    
extern struct MessageTransporter MockTransporter;
extern struct MessageTransporter DummyTransporter;
extern struct MessageTransporter MockTransporterAndHandle;
