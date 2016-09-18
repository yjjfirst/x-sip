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
    struct Dialog *dialog = AddDialog(dialogid, ua);
           
    POINTERS_EQUAL(dialog, GetDialogById(dialogid));
    DestroyUserAgent(&ua);
}

TEST(DialogsTestGroup, RemoveDialogTest)
{
    struct UserAgent *ua =  CreateUserAgent(0);
    struct DialogId *dialogid = CreateFixedDialogId((char *)"a",(char *) "b",(char *)"c");    
    AddDialog(dialogid, ua);

    RemoveDialog(dialogid);
    POINTERS_EQUAL(NULL, GetDialogById(dialogid));

    DestroyUserAgent(&ua);
}

TEST(DialogsTestGroup, RemoveDialogWithWrongId)
{
    struct UserAgent *ua =  CreateUserAgent(0);
    struct DialogId *dialogid = CreateFixedDialogId((char *)"a",(char *) "b",(char *)"c");    
    struct DialogId *dialogid2 = CreateFixedDialogId((char *)"aa",(char *) "bb",(char *)"cc");    
    struct Dialog *dialog = AddDialog(dialogid, ua);

    RemoveDialog(dialogid2);
    POINTERS_EQUAL(dialog, GetDialogById(dialogid));

    DestroyUserAgent(&ua);
    DestroyDialogId(&dialogid2);
}

TEST(DialogsTestGroup, CountDialogsTest)
{
    struct UserAgent *ua =  CreateUserAgent(0);
    struct DialogId *dialogid = CreateFixedDialogId((char *)"a",(char *) "b",(char *)"c");    
    struct DialogId *dialogid2 = CreateFixedDialogId((char *)"aa",(char *) "bb",(char *)"cc");    

    AddDialog(dialogid, ua);
    CHECK_EQUAL(1, CountDialogs());

    AddDialog(dialogid2, ua);
    CHECK_EQUAL(2, CountDialogs());
    
    DestroyUserAgent(&ua);
}

TEST(DialogsTestGroup, GetDialogTest)
{
    struct UserAgent *ua =  CreateUserAgent(0);

    struct Dialog *dialog = AddDialog(NULL_DIALOG_ID, ua);    
    struct Dialog *dialog1 = AddDialog(NULL_DIALOG_ID, ua);

    POINTERS_EQUAL(dialog, GetDialog(0));
    POINTERS_EQUAL(dialog1, GetDialog(1));

    DestroyUserAgent(&ua);
}
