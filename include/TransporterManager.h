void AddTransporter(struct MessageTransporter *t);
struct MessageTransporter *GetTransporter(int fd);
void TransporterManagerInit();
void ClearTransporterManager();
int GetMaxFd();
void FillFdset(fd_set *fdsr);
void ReceiveMessages(fd_set *fdsr);
int CountTransporter();

