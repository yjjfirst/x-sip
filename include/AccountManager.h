struct Account;

int AddAccount(struct Account *account);
void RemoveAccount(int pos);
struct Account *GetAccount(int pos);
void ClearAccountManager();
int TotalAccount();
void AccountAddBinding(int account);
void AccountRemoveBinding(int account);

extern void (*AccountInit)();
