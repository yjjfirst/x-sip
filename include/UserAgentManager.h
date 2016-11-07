struct UserAgent;
struct Account;
struct UserAgentManager;

struct UserAgent *AddUa(int account);
int CountUas();
struct UserAgent *GetUa(int pos);
int GetUaPosition(struct UserAgent *ua);
void ClearUaManager();

void RemoveUa(struct UserAgent *ua);
void DumpUaManager();
