struct Account;

int AddAccount(struct Account *account);
void RemoveAccount(int pos);
struct Account *GetAccount(int pos);
void ClearAccount();
int TotalAccount();
void BindAccount(int account);

extern void (*AccountInit)();
