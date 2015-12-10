#define T1 500
#define T2 4000
#define T4 5000

typedef void (*TimerCallback)(void *transaction);
typedef struct Timer *(*TimerAddFunc)(void *transaction, int ms, TimerCallback onTime);
typedef void (*TimerRemoveFunc)(struct Timer *t);

struct TimerManager {
    TimerAddFunc adder;
    TimerRemoveFunc remover;
};

struct TimerManager *GetTimerManager(TimerAddFunc adder, TimerRemoveFunc remover);
