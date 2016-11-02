struct Account;
struct Message;

int AddAccount(struct Account *account);
void RemoveAccount(int pos);
struct Account *GetAccount(int pos);
void ClearAccountManager();
int CountAccounts();
void AccountAddBinding(int account);
void AccountRemoveBinding(int account);
void BindAllAccounts();

int FindMessageDestAccount(struct Message *invite);

extern int (*AccountInit)();
void AccountManagerDump();
