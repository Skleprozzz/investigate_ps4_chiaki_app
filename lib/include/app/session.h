#ifdef APP_SESSION_H
#define APP_SESSION_H

#include <common.h>
#include <thread.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct app_connect_info_t
    {
        const char *host;
        const char *regist_key;
        const char *ostype;
        char auth[0x10];
        uint8_t morning[0x10];
    } AppConnectInfo;

    typedef struct app_session_t
    {
        struct
        {
            const char *host;
            const char *regist_key;
            const char *ostype;
            char auth[0x10];
            uint8_t morning[0x10];
        } connect_info;

        AppThread session_thread;
    } AppSession;

    APP_EXPORT void app_session_init(AppSession *session, AppConnectInfo *connect_info);
    APP_EXPORT void app_session_finish(AppSession *session);
    APP_EXPORT bool app_session_start(AppSession *session);
    APP_EXPORT void app_session_join(AppSession *session);

#ifdef __cplusplus
}
#endif

#endif