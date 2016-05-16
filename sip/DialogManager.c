#include <stdio.h>
#include <assert.h>

#include "Dialogs.h"
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

struct DialogManager *CreateDialogs()
{
    struct DialogManager *dialogs;
    dialogs = calloc(1, sizeof (struct DialogManager));

    return dialogs;
}

void DestoryDialogList(struct DialogManager *dialogs)
{
    int length = get_list_len(dialogs->dialogList);
    int i = 0;
    
    for (; i < length; i++) {
        struct Dialog *dialog = get_data_at(dialogs->dialogList, i);
        DestoryDialog(&dialog);
    }
}

void DestoryDialogs(struct DialogManager **dialogs)
{
    if (*dialogs != NULL) {
        DestoryDialogList(*dialogs);
        free(*dialogs);
    }
}
