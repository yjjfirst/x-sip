#include "CppUTest/TestHarness.h"

extern "C" {
#include "Dialogs.h"
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
    CreateDialog(dialogid, ua);
           
    //DialogsGetDialog(dialogid, ua);
    DestoryUserAgent(&ua);
}

TEST(DialogsTestGroup, RemoveDialTest)
{
}
