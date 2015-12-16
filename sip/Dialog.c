#include "Dialog.h"
#include "DialogId.h"

struct Dialog {
    struct DialogId *id;
};

struct Dialog *CreateDialog(struct DialogId *dialogid)
{
    struct Dialog *dialog = calloc(1, sizeof(struct Dialog));
    dialog->id =dialogid;
    return dialog;
}

struct DialogId *DialogGetId(struct Dialog *dialog)
{
    return dialog->id;
}

void DestoryDialog(struct Dialog **dialog)
{
    if (*dialog != NULL) {
        DestoryDialogId(&(*dialog)->id);
        free(*dialog);
    }
}
