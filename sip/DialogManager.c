#include <stdio.h>
#include <assert.h>

#include "DialogManager.h"
#include "Dialog.h"
#include "DialogId.h"
#include "CallIdHeader.h"
#include "utils/list/include/list.h"

#define TAG_MAX_LENGTH 64

struct DialogManager {
    t_list *dialogList;
};

struct DialogManager DialogManager;

struct Dialog *GetDialogById(struct DialogId *dialogid)
{
    int length = get_list_len(DialogManager.dialogList);
    int i = 0;
    
    assert(dialogid != NULL);

    for (; i < length ; i ++) {
        struct Dialog *dialog = get_data_at(DialogManager.dialogList, i);
        if (DialogGetId(dialog) != NULL && DialogIdMatched(DialogGetId(dialog), dialogid))
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

int CountDialogs()
{
    return get_list_len(DialogManager.dialogList);
}

bool MatchedDialogId (void *dialog, void *id)
{
    return DialogIdMatched(DialogGetId(dialog), id);
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
