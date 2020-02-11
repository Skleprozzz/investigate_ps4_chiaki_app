#include <ps4app\thread.h>

#include <stdio.h>

PS4APP_EXPORT Ps4AppErrorCode ps4app_thread_create(Ps4AppThread *thread, Ps4AppThreadFunc func, void *arg)
{
    int r = pthread_create(&thread->thread, NULL, func, arg);
    if (r != 0)
        return PS4APP_ERR_THREAD;
    return PS4APP_ERR_SUCCESS;
}

PS4APP_EXPORT Ps4AppErrorCode ps4app_thread_join(Ps4AppThread *thread, void **retval)
{
    int r = pthread_join(thread->thread, retval);
    if (r != 0)
        return PS4APP_ERR_THREAD;
    return PS4APP_ERR_SUCCESS;
}
