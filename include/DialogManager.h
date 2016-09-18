struct DialogManager;
struct DialogId;
struct CallIdHeader;

void DestroyDialogList();
struct DialogManager *CreateDialogs();
void DestroyDialogs(struct DialogManager **dialogs);

struct Dialog *GetDialogById(struct DialogId *dialogid);
void AddDialog(struct Dialog *dialog);
void RemoveDialog(struct DialogId *dialogId);
struct Dialog *GetDialog(int pos);
int CountDialogs();
