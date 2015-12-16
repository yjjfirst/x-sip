struct Dialogs;
struct DialogId;
struct CallIdHeader;

struct Dialog *GetDialogById(struct Dialogs *dialogs, struct DialogId *dialogid);
struct Dialogs *CreateDialogs();
void DestoryDialogs(struct Dialogs **dialogs);
void AddDialog(struct Dialogs *dialogs, struct Dialog *dialog);
