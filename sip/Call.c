#include "UserAgentManager.h"
#include "Dialog.h"
#include "MessageBuilder.h"
#include "TransactionManager.h"

void CallOut(int account, char *dest)
{
    struct UserAgent *ua = AddUserAgent(0);
    struct Dialog *dialog = CreateDialog(NULL_DIALOG_ID, ua);
    struct Message *invite = BuildInviteMessage(dialog);
    struct Transaction *t = AddClientInviteTransaction(invite, (struct TransactionUserObserver *)dialog);

    (void)t;
}
