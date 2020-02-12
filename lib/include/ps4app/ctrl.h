#ifndef PS4APP_CTRL_H
#define PS4APP_CTRL_H

#include "common.h"
#include "thread.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct ps4app_ctrl_t
    {
        struct ps4app_session_t *session;
        Ps4AppThread thread;
    } Ps4AppCtrl;

    PS4APP_EXPORT Ps4AppErrorCode ps4app_ctrl_start(Ps4AppCtrl *ctrl, struct ps4app_session_t *session);
    PS4APP_EXPORT Ps4AppErrorCode ps4app_ctrl_join(Ps4AppCtrl *ctrl);
    
#ifdef __cplusplus
}
#endif

#endif //PS4APP_CTRL_H