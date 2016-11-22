struct UserAgent *CallOut(int account, char *dest);
void EndCall(struct UserAgent *ua);
void AcceptCall(struct UserAgent *ua);

BOOL HandleClientMessage(char *string, char *ipAddr, int port);
