#include "CppUTest/TestHarness.h"

extern "C" {
#include "Timer.h"
}

struct Timer *TimerAdder(void *p, int ms, TimerCallback ontimer)
{
    return NULL;
}

void TimerRemover(struct Timer *p)
{
}

TEST_GROUP(TimerTestGroup)
{
};

TEST(TimerTestGroup, AddTimer)
{
    struct TimerManager *tm = GetTimerManager(TimerAdder, TimerRemover);
    (void)tm;
}
