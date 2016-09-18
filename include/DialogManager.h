struct DialogManager;
struct DialogId;
struct CallIdHeader;
struct UserAgent;

void DestroyDialogList();
struct DialogManager *CreateDialogs();
void DestroyDialogs(struct DialogManager **dialogs);

struct Dialog *GetDialogById(struct DialogId *dialogid);
struct Dialog *AddDialog(struct DialogId *dialogid, struct UserAgent *ua);
void RemoveDialog(struct DialogId *dialogId);
struct Dialog *GetDialog(int pos);
int CountDialogs();
