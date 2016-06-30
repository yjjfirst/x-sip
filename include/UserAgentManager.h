struct UserAgent;
struct Account;
struct UserAgentManager;

int AddUserAgent(struct UserAgentManager *uam, struct UserAgent *ua);
int CountUserAgent(struct UserAgentManager *uam);
struct UserAgentManager *CreateUserAgentManager();
void DestroyUserAgentManager(struct UserAgentManager **uam);
void ClearUserAgentManager(struct UserAgentManager *uam);
