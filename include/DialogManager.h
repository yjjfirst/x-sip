struct DialogManager;
struct DialogId;
struct CallIdHeader;

struct Dialog *GetDialogById(struct DialogManager *dialogs, struct DialogId *dialogid);
struct DialogManager *CreateDialogs();
void DestroyDialogs(struct DialogManager **dialogs);
void AddDialog(struct DialogManager *dialogs, struct Dialog *dialog);
void RemoveDialog(struct DialogManager *dialogs, struct DialogId *dialogId);
struct Dialog *GetDialog(struct DialogManager *dm, int pos);
int CountDialogs(struct DialogManager *dialogs);
