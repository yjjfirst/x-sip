#define T1 500
#define T2 4000
#define T4 5000

struct Timer;

typedef void (*TimerCallback)(void *transaction);
typedef void (*TimerAddFunc)(void *transaction, int ms, TimerCallback onTime);
typedef void (*TimerRemoveFunc)(struct Timer *t);

struct TimerManager *CreateTimerManager(TimerAddFunc adder, TimerRemoveFunc remover);
