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

    DestoryDialogs(&dialogs);
}

TEST(DialogsTestGroup, AddDialogTest)
{
    struct UserAgent *ua =  CreateUserAgent();
    struct DialogId *dialogid = CreateDialogId((char *)"a",(char *) "b",(char *)"c");    
    struct Dialog *dialog = CreateDialog(dialogid, ua);
           
    POINTERS_EQUAL(dialog, UserAgentGetDialog(ua, dialogid));
    DestoryUserAgent(&ua);
}

TEST(DialogsTestGroup, RemoveDialogTest)
{
    struct UserAgent *ua =  CreateUserAgent();
    struct DialogId *dialogid = CreateDialogId((char *)"a",(char *) "b",(char *)"c");    
    CreateDialog(dialogid, ua);

    UserAgentRemoveDialog(ua, dialogid);
    POINTERS_EQUAL(NULL, UserAgentGetDialog(ua, dialogid));

    DestoryUserAgent(&ua);
}

TEST(DialogsTestGroup, RemoveDialogWithWrongId)
{
    struct UserAgent *ua =  CreateUserAgent();
    struct DialogId *dialogid = CreateDialogId((char *)"a",(char *) "b",(char *)"c");    
    struct DialogId *dialogid2 = CreateDialogId((char *)"aa",(char *) "bb",(char *)"cc");    
    struct Dialog *dialog = CreateDialog(dialogid, ua);

    UserAgentRemoveDialog(ua, dialogid2);
    POINTERS_EQUAL(dialog, UserAgentGetDialog(ua, dialogid));

    DestoryUserAgent(&ua);
    DestoryDialogId(&dialogid2);
}
