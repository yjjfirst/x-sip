#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "AccountManager.h"
#include "Accounts.h"
#include "UserAgentManager.h"
#include "Dialog.h"
#include "MessageBuilder.h"
#include "Messages.h"
#include "UserAgent.h"
#include "TransactionManager.h"
#include "URI.h"
#include "ContactHeader.h"
#include "Header.h"
#include "DialogManager.h"
#include "Transaction.h"
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

int TotalAccount()
{
    struct AccountManager *am = &AccountManager;
    return get_list_len(am->accounts);
}

void ClearAccountManager()
{
    int i = 0;
    struct AccountManager *am = &AccountManager;
    int len = TotalAccount();

    for (; i < len; i++) {
        struct Account *a = (struct Account *)get_data_at(am->accounts, i);
        DestroyAccount(&a);
    }
    
    destroy_list(&am->accounts, NULL);
}

int FindAccountByUserName(char *user)
{
    for (int i =0; i < TotalAccount(); i++) {
        struct Account *account = GetAccount(i);
        if (strcmp(user, AccountGetUserName(account)) == 0)
            return i;
    }

    return -1;
}

void AccountAddBinding(int account)
{
    struct UserAgent *ua = AddUserAgent(account);
    struct Dialog *dialog = AddDialog(NULL_DIALOG_ID, ua);
    MESSAGE *message = BuildAddBindingMessage(dialog);

    DialogNewTransaction(dialog, message, TRANSACTION_TYPE_CLIENT_NON_INVITE);
}

void AccountRemoveBinding(int account)
{
    struct UserAgent *ua = AddUserAgent(account);
    struct Dialog *dialog = AddDialog(NULL_DIALOG_ID, ua);
    MESSAGE *message = BuildRemoveBindingMessage(dialog);

    DialogNewTransaction(dialog, message, TRANSACTION_TYPE_CLIENT_NON_INVITE);
}

void BindAllAccounts()
{
    int i = 0;

    for (; i < TotalAccount(); i++) {
        AccountAddBinding(i);
    }
}

int FindMessageDestAccount(struct Message *invite)
{
    CONTACT_HEADER *to = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_TO, invite);
    URI *uri = ContactHeaderGetUri(to);
    char *user = UriGetUser(uri);

    return FindAccountByUserName(user);
}


int AccountInitImpl()
{
    ClearAccountManager();

    AddAccount(CreateAccount(
                   (char *)"88001", 
                   (char *)"88001", 
                   (char *)"88001", 
                   (char *)"192.168.10.62", 
                   (char *)"192.168.10.62"));
    
    AddAccount(CreateAccount(
                   (char *)"88002", 
                   (char *)"88002", 
                   (char *)"88002", 
                   (char *)"192.168.10.62", 
                   (char *)"192.168.10.62"));
    
    AddAccount(CreateAccount(
                   (char *)"88003", 
                   (char *)"88003", 
                   (char *)"88003", 
                   (char *)"192.168.10.62", 
                   (char *)"192.168.10.62"));
    
    return TotalAccount();
}

int (*AccountInit)() = AccountInitImpl;

void AccountManagerDump()
{
    int i = 0;
    struct AccountManager *am = &AccountManager;
    int len = get_list_len(am->accounts);

    printf("\n");
    for (; i < len; i++) {
        struct Account *a = (struct Account *)get_data_at(am->accounts, i);
        AccountDump(a);
    }

}
