struct Account;

struct AccountManager *AccountManagerGet();
void AddAccount(struct AccountManager *am, struct Account *account);
void RemoveAccount(struct AccountManager *am, int pos);
struct Account *GetAccount(struct AccountManager *am, int pos);
void ClearAccount(struct AccountManager *am);
int TotalAccount(struct AccountManager *am);

extern void (*AccountInit)(struct AccountManager *am);
