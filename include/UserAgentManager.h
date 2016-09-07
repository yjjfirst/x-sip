struct UserAgent;
struct Account;
struct UserAgentManager;

struct UserAgent *AddUserAgent(int account);
int CountUserAgent();
struct UserAgent *GetUserAgent(int pos);
void ClearUserAgentManager();

void UserAgentManagerDump();
