#ifdef BUILD_LOG
#define LOG(fmt, ...) \
    do { printf(fmt, __VA_ARGS__);} while (0)    
#else
#define LOG(fmt, ...) \
    do {} while (0)
#endif
