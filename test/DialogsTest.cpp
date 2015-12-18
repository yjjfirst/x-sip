#include "CppUTest/TestHarness.h"

extern "C" {
#include "Dialogs.h"
#include "Dialog.h"
#include "DialogId.h"
}

TEST_GROUP(DialogsTestGroup)
{
};

TEST(DialogsTestGroup, CreateDialogsTest)
{
    struct Dialogs *dialogs = CreateDialogs();
    
    CHECK_TRUE(dialogs != NULL);

    DestoryDialogs(&dialogs);
}

TEST(DialogsTestGroup, AddDialogTest)
{
    struct Dialogs *dialogs = CreateDialogs();
    struct DialogId *dialogid = CreateDialogId((char *)"a",(char *) "b",(char *)"c");    
    struct Dialog *dialog = CreateDialog(dialogid, NULL);
           
    AddDialog(dialogs, dialog);

    DestoryDialogs(&dialogs);
}

