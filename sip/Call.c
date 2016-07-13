#include "UserAgentManager.h"
#include "UserAgent.h"
#include "Dialog.h"
#include "MessageBuilder.h"
#include "TransactionManager.h"

struct UserAgent *CallOut(int account, char *dest)
{
    struct UserAgent *ua = AddUserAgent(0);
    struct Dialog *dialog = AddNewDialog(NULL_DIALOG_ID, ua);
    struct Message *invite = BuildInviteMessage(dialog);

    AddClientInviteTransaction(invite, (struct TransactionUserObserver *)dialog);

    return ua;
}

void EndCall(struct UserAgent *ua)
{
    struct Dialog *dialog = UserAgentGetDialog(ua, 0);
    struct Message *bye = BuildByeMessage(dialog);
    
    AddClientInviteTransaction(bye, (struct TransactionUserObserver *)dialog);
}
