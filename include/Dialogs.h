struct DialogManager;
struct DialogId;
struct CallIdHeader;

struct Dialog *GetDialogById(struct DialogManager *dialogs, struct DialogId *dialogid);
struct DialogManager *CreateDialogs();
void DestoryDialogs(struct DialogManager **dialogs);
void AddDialog(struct DialogManager *dialogs, struct Dialog *dialog);
