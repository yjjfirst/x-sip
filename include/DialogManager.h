struct DialogManager;
struct DialogId;
struct CallIdHeader;

struct Dialog *GetDialogById(struct DialogManager *dialogs, struct DialogId *dialogid);
struct DialogManager *CreateDialogs();
void DestroyDialogs(struct DialogManager **dialogs);
void AddDialog(struct DialogManager *dialogs, struct Dialog *dialog);
void RemoveDialog(struct DialogManager *dialogs, struct DialogId *dialogId);
int CountDialogs(struct DialogManager *dialogs);
