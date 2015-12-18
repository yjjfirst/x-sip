#include "Dialog.h"
#include "DialogId.h"

struct Dialog {
    struct DialogId *id;
    struct UserAgent *ua;
};

struct Dialog *CreateDialog(struct DialogId *dialogid, struct UserAgent *ua)
{
    struct Dialog *dialog = calloc(1, sizeof(struct Dialog));
    dialog->id = dialogid;
    dialog->ua = ua;
    return dialog;
}

struct DialogId *DialogGetId(struct Dialog *dialog)
{
    return dialog->id;
}

struct UserAgent *DialogGetUserAgent(struct Dialog *dialog)
{
    return dialog->ua;
}

char *DialogGetToUser(struct Dialog *dialog)
{
    return "88002";
}

void DestoryDialog(struct Dialog **dialog)
{
    if (*dialog != NULL) {
        DestoryDialogId(&(*dialog)->id);
        free(*dialog);
    }
}
