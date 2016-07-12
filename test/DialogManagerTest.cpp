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
    struct Dialog *dialog = CreateDialog(dialogid, ua);
           
    POINTERS_EQUAL(dialog, UserAgentGetDialog(ua, dialogid));
    DestroyUserAgent(&ua);
}

TEST(DialogsTestGroup, RemoveDialogTest)
{
    struct UserAgent *ua =  CreateUserAgent(0);
    struct DialogId *dialogid = CreateFixedDialogId((char *)"a",(char *) "b",(char *)"c");    
    CreateDialog(dialogid, ua);

    UserAgentRemoveDialog(ua, dialogid);
    POINTERS_EQUAL(NULL, UserAgentGetDialog(ua, dialogid));

    DestroyUserAgent(&ua);
}

TEST(DialogsTestGroup, RemoveDialogWithWrongId)
{
    struct UserAgent *ua =  CreateUserAgent(0);
    struct DialogId *dialogid = CreateFixedDialogId((char *)"a",(char *) "b",(char *)"c");    
    struct DialogId *dialogid2 = CreateFixedDialogId((char *)"aa",(char *) "bb",(char *)"cc");    
    struct Dialog *dialog = CreateDialog(dialogid, ua);

    UserAgentRemoveDialog(ua, dialogid2);
    POINTERS_EQUAL(dialog, UserAgentGetDialog(ua, dialogid));

    DestroyUserAgent(&ua);
    DestroyDialogId(&dialogid2);
}

TEST(DialogsTestGroup, CountDialogsTest)
{
    struct UserAgent *ua =  CreateUserAgent(0);
    struct DialogId *dialogid = CreateFixedDialogId((char *)"a",(char *) "b",(char *)"c");    
    struct DialogId *dialogid2 = CreateFixedDialogId((char *)"aa",(char *) "bb",(char *)"cc");    

    CreateDialog(dialogid, ua);
    CHECK_EQUAL(1, CountDialogs(UserAgentGetDialogManager(ua)));

    CreateDialog(dialogid2, ua);
    CHECK_EQUAL(2, CountDialogs(UserAgentGetDialogManager(ua)));
    
    DestroyUserAgent(&ua);
}
