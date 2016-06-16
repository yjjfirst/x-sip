struct Account;

void AddAccount(struct Account *account);
void RemoveAccount(int pos);
struct Account *GetAccount(int pos);
void ClearAccount();
int TotalAccount();

extern void (*AccountInit)();
