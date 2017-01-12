struct UserAgent;
struct Account;
struct UserAgentManager;
struct Message;

struct UserAgent *AddUa(int account);
struct UserAgent *GetUa(int pos);
struct UserAgent *PassiveAddUa(struct Message *message);

int CountUas();
int GetUaPosition(struct UserAgent *ua);
extern void (*RemoveUa)(struct UserAgent *ua);

void ClearUaManager();
void DumpUaManager();
