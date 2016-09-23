#include <stdio.h>
#include <assert.h>

#include "SipUdp.h"
#include "Transporter.h"
#include "utils/list/include/list.h"
#include "Messages.h"

struct TransporterManager {
    t_list *transporters;
};

static struct TransporterManager TransporterManager;

void AddTransporter(struct MessageTransporter *t)
{
    assert (t != NULL);

    if (t->init != NULL)
        t->fd = t->init();
    
    put_in_list(&TransporterManager.transporters, t);
}

int CountTransporter()
{
    return get_list_len(TransporterManager.transporters);
}

struct MessageTransporter *GetTransporter(int fd)
{
    int i = 0;
    int len = CountTransporter();
    struct MessageTransporter *t = NULL;
        
    for (; i < len; i++) {
        t = (struct MessageTransporter *)get_data_at(TransporterManager.transporters, i);
        if (fd == t->fd) return t;
    }

    return NULL;
}

void TransporterManagerInit()
{
    AddTransporter(&SipUdpTransporter);
    AddTransporter(&ClientTransporter);
}

void ClearTransporterManager()
{
    destroy_list(&TransporterManager.transporters, NULL);
}

int GetMaxFd()
{
    int i = 0;
    int len = CountTransporter();
    int max = 0;
    struct MessageTransporter *t = NULL;
        
    for (; i < len; i++) {
        t = (struct MessageTransporter *)get_data_at(TransporterManager.transporters, i);
        if (t->fd > max) max = t->fd;
    }

    return max;
}

void FillFdset(fd_set *fdsr)
{
    int i = 0;
    int len = CountTransporter();
    struct MessageTransporter *t = NULL;

    FD_ZERO(fdsr);
   
    for (; i < len; i++) {
        t = (struct MessageTransporter *)get_data_at(TransporterManager.transporters, i);
        FD_SET(t->fd, fdsr);
    }
}

void ReceiveMessages(fd_set *fdsr)
{
    int i = 0;
    int len = CountTransporter();
    struct MessageTransporter *t = NULL;
    char message[MAX_MESSAGE_LENGTH] = {0};

    for (; i < len; i++) {        
        t = (struct MessageTransporter *)get_data_at(TransporterManager.transporters, i);
        if (FD_ISSET(t->fd, fdsr)) {
            if (t->receive != NULL) t->receive(message, t->fd);
            if (t->callback != NULL) t->callback(message);
        }
    }    
}
