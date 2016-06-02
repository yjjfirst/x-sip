
#include "AccountManager.h"
#include "Accounts.h"
#include "utils/list/include/list.h"

struct AccountManager {
    t_list *accounts;
};

struct AccountManager AccountManager;

struct AccountManager *AccountManagerGet()
{
    return &AccountManager;
}

void AddAccount(struct AccountManager *am, struct Account *account)
{
    put_in_list(&am->accounts, account);
}

struct Account *GetAccount(struct AccountManager *am, int pos)
{
    return get_data_at(am->accounts, pos);
}

void ClearAccount(struct AccountManager *am)
{
    int i = 0;
    int len = get_list_len(am->accounts);

    for (; i < len; i++) {
        struct Account *a = (struct Account *)get_data_at(am->accounts, i);
        DestoryAccount(&a);
    }
    
    destroy_list(&am->accounts, NULL);
}

int TotalAccount(struct AccountManager *am)
{
    return get_list_len(am->accounts);
}

