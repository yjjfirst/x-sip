#include <stdio.h>
#include <assert.h>

#include "DialogManager.h"
#include "Dialog.h"
#include "DialogId.h"
#include "CallIdHeader.h"
#include "Messages.h"
#include "utils/list/include/list.h"

#define TAG_MAX_LENGTH 64

struct DialogManager {
    t_list *dialogList;
};

struct DialogManager DialogManager;

struct Dialog *GetDialogByUserAgent(struct UserAgent *ua)
{
    int length = get_list_len(DialogManager.dialogList);
    int i = 0;
    
    for (; i < length ; i ++) {
        struct Dialog *dialog = get_data_at(DialogManager.dialogList, i);
        if (DialogGetUserAgent(dialog) == ua) {
            return dialog;
        }
    }

    return NULL;

}

struct Dialog *MatchMessage2Dialog(struct Message *message)
{
    char *localTag = MessageGetToTag(message);
    char *remoteTag = MessageGetFromTag(message);
    char *callid = MessageGetCallId(message);

    struct DialogId *id = CreateFixedDialogId(callid, localTag, remoteTag);

    return GetDialogById(id);
}

struct Dialog *GetDialogById(struct DialogId *dialogid)
{
    int length = get_list_len(DialogManager.dialogList);
    int i = 0;
    
    assert(dialogid != NULL);

    for (; i < length ; i ++) {
        struct Dialog *dialog = get_data_at(DialogManager.dialogList, i);
        if (GetDialogId(dialog) != NULL && DialogIdMatched(GetDialogId(dialog), dialogid))
           return dialog;
    }

    return NULL;
}

struct Dialog *GetDialog(int pos)
{
    return get_data_at(DialogManager.dialogList, pos);
}

struct Dialog *AddDialog(struct DialogId *dialogid, struct UserAgent *ua)
{
    struct Dialog *dialog = CreateDialog(dialogid, ua);
    put_in_list(&DialogManager.dialogList, dialog);
    
    return dialog;
}

struct Dialog *AddConfirmedDialog(struct DialogId *dialogid, struct UserAgent *ua)
{
    struct Dialog *dialog = AddDialog(dialogid, ua);
    SetDialogState(dialog, DIALOG_STATE_CONFIRMED);

    return dialog;
}

int CountDialogs()
{
    return get_list_len(DialogManager.dialogList);
}

bool MatchedDialogId (void *dialog, void *id)
{
    return DialogIdMatched(GetDialogId(dialog), id);
}

void RemoveDialog(struct DialogId *dialogId)
{
    struct Dialog *dialog = del_node_as_arg(&DialogManager.dialogList, MatchedDialogId, dialogId);
    DestroyDialog(&dialog);
}

struct DialogManager *CreateDialogs()
{
    struct DialogManager *dialogs;
    dialogs = calloc(1, sizeof (struct DialogManager));

    return dialogs;
}

void DestroyDialogList()
{
    int length = get_list_len(DialogManager.dialogList);
    int i = 0;
    
    for (; i < length; i++) {
        struct Dialog *dialog = get_data_at(DialogManager.dialogList, i);
        DestroyDialog(&dialog);
    }

    destroy_list(&DialogManager.dialogList, NULL);
    DialogManager.dialogList = NULL;
}

void ClearDialogManager()
{
    DestroyDialogList();
}

void DestroyDialogs(struct DialogManager **dialogs)
{
    struct DialogManager *dm = *dialogs;
    if (dm != NULL) {
        DestroyDialogList(dm);
        free(dm);
        *dialogs = NULL;
    }
}

void DumpDialogManager()
{
    int length = get_list_len(DialogManager.dialogList);
    int i = 0;
    
    printf("\n");
    for (; i < length; i++) {
        struct Dialog *dialog = get_data_at(DialogManager.dialogList, i);
        DumpDialog(dialog);
    }
}
