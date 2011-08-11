//#define DEBUG
#ifdef DEBUG
#define DBG(msg, args...) { printf(msg, ## args); printf("\n"); }
#else
#define DBG(msg, args...) { }
#endif

/*#define DBG1(msg, args...) { char _ts[20] = {0}; time_t _t; struct tm _tm; \
                            time(&_t); localtime_r(&_t, &_tm);            \
                            strftime(_ts, 19, "%b %d %H:%M:%S", &_tm);    \
                            printf("%s: DEBUG: " __FILE__ ": ", _ts);     \
                            printf(msg, ## args); printf("\n"); }
*/


