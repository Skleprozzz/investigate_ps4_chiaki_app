#ifndef PS4APP_THREAD_H
#define PS4APP_THREAD_H

#include "common.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include <pthread.h>

    typedef struct ps4app_thread_t
    {
        pthread_t thread;
    } Ps4AppThread;
    typedef void *(*Ps4AppThreadFunc)(void *);

    PS4APP_EXPORT Ps4AppErrorCode ps4app_thread_create(Ps4AppThread *thread, Ps4AppThreadFunc func, void *arg);
    PS4APP_EXPORT Ps4AppErrorCode ps4app_thread_join(Ps4AppThread *thred, void **retval);

#ifdef __cplusplus
}

#endif

#endif