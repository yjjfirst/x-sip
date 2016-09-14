struct Account;
struct Message;

int AddAccount(struct Account *account);
void RemoveAccount(int pos);
struct Account *GetAccount(int pos);
void ClearAccountManager();
int TotalAccount();
void AccountAddBinding(int account);
void AccountRemoveBinding(int account);
void BindAllAccounts();

struct Account *FindMessageDestAccount(struct Message *invite);

extern int (*AccountInit)();
void AccountManagerDump();
