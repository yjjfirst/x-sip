#include <stdio.h>
#include <unistd.h>

#include "AccountManager.h"
#include "Transporter.h"

void InitStack()
{
    if (fork() != 0) {
        printf("main thread\n");
        return;
    }

    AccountInit();
    TransporterInit();
    BindAllAccounts();
    while(1) {
        ReceiveInMessage();
    }
}

