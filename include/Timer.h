#define T1 500
#define T2 4000
#define T4 5000

#define TRANSPORT_TIMEOUT_INTERVAL 64*T1

typedef void (*TimerCallback)(void *data);
typedef struct Timer *(*TimerAddFunc)(void *data, int ms, TimerCallback action);
typedef void (*TimerRemoveFunc)(struct Timer *t);

extern struct Timer *(*AddTimer)(void *data, int ms, TimerCallback action);
extern void (*RemoveTimer)(struct Timer *t);
