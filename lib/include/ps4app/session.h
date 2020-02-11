#ifndef PS4APP_SESSION_H
#define PS4APP_SESSION_H

#include "common.h"
#include "thread.h"

#include <stdint.h>
#include <Ws2tcpip.h>
#include <winsock.h>


#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct ps4app_connect_info_t
    {
        const char *host;
        const char *regist_key;
        const char *ostype;
        char auth[0x10];
        uint8_t morning[0x10];
    } Ps4AppConnectInfo;

    typedef struct ps4app_session_t
    {
        struct
        {
            struct addrinfo *host_addrinfos;
            struct addrinfo *host_addrinfo_selected;
            char *regist_key;
            char *ostype;
            char auth[0x10];
            uint8_t morning[0x10];
        } connect_info;

        Ps4AppThread session_thread;
    } Ps4AppSession;

    PS4APP_EXPORT Ps4AppErrorCode ps4app_session_init(Ps4AppSession *session, Ps4AppConnectInfo *connect_info);
    PS4APP_EXPORT void ps4app_session_finish(Ps4AppSession *session);
    PS4APP_EXPORT Ps4AppErrorCode ps4app_session_start(Ps4AppSession *session);
    PS4APP_EXPORT Ps4AppErrorCode ps4app_session_join(Ps4AppSession *session);

#ifdef __cplusplus
}
#endif

#endif