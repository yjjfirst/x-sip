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

struct Dialog *GetDialogById(struct DialogManager *dialogs, struct DialogId *dialogid)
{
    int length = get_list_len(dialogs->dialogList);
    int i = 0;
    
    assert(dialogs != NULL);
    assert(dialogid != NULL);

    for (; i < length ; i ++) {
        struct Dialog *dialog = get_data_at(dialogs->dialogList, i);
        if (DialogGetId(dialog) != NULL && DialogIdMatched(DialogGetId(dialog), dialogid))
           return dialog;
    }

    return NULL;
}

void AddDialog(struct DialogManager *dialogs, struct Dialog *dialog)
{
    put_in_list(&dialogs->dialogList, dialog);
}

bool MatchedDialogId (void *dialog, void *id)
{
    return DialogIdMatched(DialogGetId(dialog), id);
}

void RemoveDialog(struct DialogManager *dialogs, struct DialogId *dialogId)
{
    struct Dialog *dialog = del_node_as_arg(&dialogs->dialogList, MatchedDialogId, dialogId);
    DestroyDialog(&dialog);
}

struct DialogManager *CreateDialogs()
{
    struct DialogManager *dialogs;
    dialogs = calloc(1, sizeof (struct DialogManager));

    return dialogs;
}

void DestroyDialogList(struct DialogManager *dialogs)
{
    int length = get_list_len(dialogs->dialogList);
    int i = 0;
    
    for (; i < length; i++) {
        struct Dialog *dialog = get_data_at(dialogs->dialogList, i);
        DestroyDialog(&dialog);
    }

    destroy_list(&dialogs->dialogList, NULL);
    dialogs->dialogList = NULL;
}

void DestroyDialogs(struct DialogManager **dialogs)
{
    if (*dialogs != NULL) {
        DestroyDialogList(*dialogs);
        free(*dialogs);
        *dialogs = NULL;
    }
}
