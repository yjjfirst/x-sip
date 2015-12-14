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
    struct Dialog *dialog = CreateDialog();
    char callid[] = "";
    char localTag[] = "";
    char remoteTag[] = "";
            
    AddDialog(dialogs, dialog);
    GetDialogByCallId(dialogs, callid, localTag, remoteTag);
    DestoryDialogs(&dialogs);
}

