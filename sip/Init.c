#include <stdio.h>
#include <unistd.h>

#include "AccountManager.h"
#include "Transporter.h"
#include "TransporterManager.h"

void InitStack()
{
    fd_set fdsr;
    struct timeval tv;
    
    if (fork() != 0) {
        printf("main thread\n");
        return;
    }

    AccountInit();
    TransporterManagerInit();
    BindAllAccounts();
    
    while(1) {
        FillFdset(&fdsr);

        tv.tv_sec = 1;
        tv.tv_usec = 0;

        select(GetMaxFd() + 1, &fdsr, NULL, NULL, &tv);
        ReceiveMessages(&fdsr);
    }
}

