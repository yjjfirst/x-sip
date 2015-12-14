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

TEST(DialogsTestGroup, CreateDialogIdTest)
{
    char callid[] = "1";
    char localTag[] = "2";
    char remoteTag[] = "3";

    struct DialogId *dialogId = CreateDialogId(callid, localTag, remoteTag);
    STRCMP_EQUAL("1", DialogIdGetCallId(dialogId));
    STRCMP_EQUAL("2", DialogIdGetLocalTag(dialogId));
    STRCMP_EQUAL("3", DialogIdGetRemoteTag(dialogId));

    DestoryDialogId(&dialogId);
}

TEST(DialogsTestGroup, DialogIdEqualTest)
{
    struct DialogId *id1 = CreateDialogId((char *)"1", (char *)"2", (char *)"3");
    struct DialogId *id2 = CreateDialogId((char *)"1", (char *)"2", (char *)"3");

    CHECK_TRUE(DialogIdMatched(id1, id2));

    DestoryDialogId(&id1);
    DestoryDialogId(&id2);
}

TEST(DialogsTestGroup, DialogIdUnequalTest)
{
    struct DialogId *id1 = CreateDialogId((char *)"2", (char *)"2", (char *)"3");
    struct DialogId *id2 = CreateDialogId((char *)"1", (char *)"2", (char *)"3");

    CHECK_FALSE(DialogIdMatched(id1, id2));

    DestoryDialogId(&id1);
    DestoryDialogId(&id2);
}

TEST(DialogsTestGroup, DialogIdUnequalTest1)
{
    struct DialogId *id1 = CreateDialogId((char *)"4", (char *)"5", (char *)"6");
    struct DialogId *id2 = CreateDialogId((char *)"1", (char *)"2", (char *)"3");

    CHECK_FALSE(DialogIdMatched(id1, id2));

    DestoryDialogId(&id1);
    DestoryDialogId(&id2);
}

TEST(DialogsTestGroup, DialogIdUnequalTest2)
{
    struct DialogId *id1 = CreateDialogId((char *)"1", (char *)"1", (char *)"6");
    struct DialogId *id2 = CreateDialogId((char *)"1", (char *)"2", (char *)"3");

    CHECK_FALSE(DialogIdMatched(id1, id2));

    DestoryDialogId(&id1);
    DestoryDialogId(&id2);
}
