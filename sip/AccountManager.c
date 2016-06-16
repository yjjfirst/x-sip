#include <assert.h>
#include "AccountManager.h"
#include "Accounts.h"
#include "utils/list/include/list.h"

struct AccountManager {
    t_list *accounts;
};

struct AccountManager AccountManager;

void AddAccount(struct Account *account)
{
    struct AccountManager *am = &AccountManager;
    put_in_list(&am->accounts, account);
}

struct Account *GetAccount(int pos)
{
    struct AccountManager *am = &AccountManager;
    return get_data_at(am->accounts, pos);
}

void RemoveAccount(int pos)
{
    struct AccountManager *am = &AccountManager;
    struct Account *a = (struct Account *)get_data_at(am->accounts, pos);
    DestoryAccount(&a);
    del_node_at(&am->accounts, pos);
}

void ClearAccount()
{
    int i = 0;
    struct AccountManager *am = &AccountManager;
    int len = get_list_len(am->accounts);

    assert(am != NULL);
    for (; i < len; i++) {
        struct Account *a = (struct Account *)get_data_at(am->accounts, i);
        DestoryAccount(&a);
    }
    
    destroy_list(&am->accounts, NULL);
}

int TotalAccount()
{
    struct AccountManager *am = &AccountManager;
    return get_list_len(am->accounts);
}

void AccountInitImpl()
{
}

void (*AccountInit)() = AccountInitImpl;