#include "CppUTest/TestHarness.h"

extern "C" {
#include "DialogManager.h"
#include "Dialog.h"
#include "DialogId.h"
#include "UserAgent.h"
}

TEST_GROUP(DialogsTestGroup)
{
};

TEST(DialogsTestGroup, CreateDialogsTest)
{
    struct DialogManager *dialogs = CreateDialogs();
    
    CHECK_TRUE(dialogs != NULL);

    DestroyDialogs(&dialogs);
}

TEST(DialogsTestGroup, AddDialogTest)
{
    struct UserAgent *ua =  CreateUserAgent(0);
    struct DialogId *dialogid = CreateFixedDialogId((char *)"a",(char *) "b",(char *)"c");    
    struct Dialog *dialog = AddNewDialog(dialogid, ua);
           
    POINTERS_EQUAL(dialog, UserAgentGetDialogById(ua, dialogid));
    DestroyUserAgent(&ua);
}

TEST(DialogsTestGroup, RemoveDialogTest)
{
    struct UserAgent *ua =  CreateUserAgent(0);
    struct DialogId *dialogid = CreateFixedDialogId((char *)"a",(char *) "b",(char *)"c");    
    AddNewDialog(dialogid, ua);

    UserAgentRemoveDialog(ua, dialogid);
    POINTERS_EQUAL(NULL, UserAgentGetDialogById(ua, dialogid));

    DestroyUserAgent(&ua);
}

TEST(DialogsTestGroup, RemoveDialogWithWrongId)
{
    struct UserAgent *ua =  CreateUserAgent(0);
    struct DialogId *dialogid = CreateFixedDialogId((char *)"a",(char *) "b",(char *)"c");    
    struct DialogId *dialogid2 = CreateFixedDialogId((char *)"aa",(char *) "bb",(char *)"cc");    
    struct Dialog *dialog = AddNewDialog(dialogid, ua);

    UserAgentRemoveDialog(ua, dialogid2);
    POINTERS_EQUAL(dialog, UserAgentGetDialogById(ua, dialogid));

    DestroyUserAgent(&ua);
    DestroyDialogId(&dialogid2);
}

TEST(DialogsTestGroup, CountDialogsTest)
{
    struct UserAgent *ua =  CreateUserAgent(0);
    struct DialogId *dialogid = CreateFixedDialogId((char *)"a",(char *) "b",(char *)"c");    
    struct DialogId *dialogid2 = CreateFixedDialogId((char *)"aa",(char *) "bb",(char *)"cc");    

    AddNewDialog(dialogid, ua);
    CHECK_EQUAL(1, CountDialogs(UserAgentGetDialogManager(ua)));

    AddNewDialog(dialogid2, ua);
    CHECK_EQUAL(2, CountDialogs(UserAgentGetDialogManager(ua)));
    
    DestroyUserAgent(&ua);
}

TEST(DialogsTestGroup, GetDialogTest)
{
    struct UserAgent *ua =  CreateUserAgent(0);

    struct Dialog *dialog = AddNewDialog(NULL_DIALOG_ID, ua);    
    struct Dialog *dialog1 = AddNewDialog(NULL_DIALOG_ID, ua);
    struct DialogManager *dm = UserAgentGetDialogManager(ua);

    POINTERS_EQUAL(dialog, GetDialog(dm, 0));
    POINTERS_EQUAL(dialog1, GetDialog(dm, 1));

    DestroyUserAgent(&ua);
}
