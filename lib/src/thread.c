#include <app\thread.h>

#include <stdio.h>

APP_EXPORT bool app_thread_create(AppThread *thread, AppThreadFunc func, void *arg)
{
    int r = pthread_create(&thread->thread, NULL, func, arg);
    if (!r)
    {
        perror("pthread_create");
        return false;
    }
    return true;
}

APP_EXPORT bool app_thread_join(AppThread *thread, void **retval)
{
    int r = pthread_join(&thread->thread, retval);
    if (r != 0)
    {
        perror("pthread_join");
        return false;
    }
    return true;
}
