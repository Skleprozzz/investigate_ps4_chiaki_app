#ifdef APP_THREAD_H
#define APP_THREAD_H

#include <common.h>

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include <pthread.h>

    typedef struct app_thread_t
    {
        pthread_t thread;
    } AppThread;
    typedef void *(*AppThreadFunk)(void *);

    APP_EXPORT bool app_thread_create(AppThread *thread, PS4ThreadFunc func, void *arg);
    APP_EXPORT bool app_thread_join(AppThread *thred, void **retval);

#ifdef __cplusplus
}

#endif

#endif