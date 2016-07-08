#include <assert.h>
#include "AccountManager.h"
#include "Accounts.h"
#include "UserAgentManager.h"
#include "Dialog.h"
#include "MessageBuilder.h"
#include "Messages.h"
#include "UserAgent.h"
#include "TransactionManager.h"
#include "utils/list/include/list.h"

struct AccountManager {
    t_list *accounts;
};

struct AccountManager AccountManager;

int AddAccount(struct Account *account)
{
    struct AccountManager *am = &AccountManager;
    int pos;

    put_in_list(&am->accounts, account);    
    pos = get_list_len(am->accounts) - 1; 

    return pos;
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
    DestroyAccount(&a);
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
        DestroyAccount(&a);
    }
    
    destroy_list(&am->accounts, NULL);
}

int TotalAccount()
{
    struct AccountManager *am = &AccountManager;
    return get_list_len(am->accounts);
}

void AccountAddBinding(int account)
{
    struct UserAgent *ua = AddUserAgent(account);
    struct Dialog *dialog = CreateDialog(NULL_DIALOG_ID, ua);
    struct Message *message = BuildAddBindingMessage(dialog);

    AddClientNonInviteTransaction(message, (struct TransactionUserObserver *)dialog);
}

void AccountRemoveBinding(int account)
{
    struct UserAgent *ua = AddUserAgent(account);
    struct Dialog *dialog = CreateDialog(NULL_DIALOG_ID, ua);
    struct Message *message = BuildRemoveBindingMessage(dialog);

    AddClientNonInviteTransaction(message, (struct TransactionUserObserver *)dialog);
}

void AccountInitImpl()
{
}

void (*AccountInit)() = AccountInitImpl;
