#include "Timer.h"

struct TimerManager {
    TimerAddFunc adder;
    TimerRemoveFunc remover;
};

static struct TimerManager TimerManager;

struct TimerManager *GetTimerManager(TimerAddFunc adder, TimerRemoveFunc remover)
{
    TimerManager.adder = adder;
    TimerManager.remover = remover;
    return &TimerManager;
}
