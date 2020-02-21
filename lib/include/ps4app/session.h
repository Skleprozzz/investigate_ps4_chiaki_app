#ifndef PS4APP_SESSION_H
#define PS4APP_SESSION_H

#include "common.h"
#include "thread.h"
#include "log.h"
#include "ctrl.h"
#include "rpcrypt.h"


#include <stdint.h>
#include <stdint.h>
#include <netdb.h>

#ifdef __cplusplus
extern "C"
{
#endif


#define PS4APP_RP_DID_SIZE 32

  typedef struct ps4app_connect_info_t
  {
    const char *host;// null terminated
    const char *regist_key;// null terminated
    const char *ostype;// null terminated
    char auth[0x10];// null terminated
    uint8_t morning[0x10];  // must be completely filled (pad with \0)
  } Ps4AppConnectInfo;
  typedef enum
  {
    PS4APP_QUIT_REASON_NONE,
    PS4APP_QUIT_REASON_SESSION_REQUEST_UNKNOWN,
    PS4APP_QUIT_REASON_SESSION_REQUEST_CONNECTION_REFUSED,
    PS4APP_QUIT_REASON_SESSION_REQUEST_RP_IN_USE,
    PS4APP_QUIT_REASON_SESSION_REQUEST_RP_CRASH,
    PS4APP_QUIT_REASON_CTRL_UNKNOWN,
    PS4APP_QUIT_REASON_CTRL_CONNECTION_REFUSED
  } Ps4AppQuitReason;

  typedef struct ps4app_quit_event_t
  {
    Ps4AppQuitReason reason;
  } Ps4AppQuitEvent;

  typedef enum
  {
    PS4APP_EVENT_QUIT
  } Ps4AppEventType;

  typedef struct ps4app_event_t
  {
    Ps4AppEventType type;
    union {
      Ps4AppQuitEvent quit;
    };
  } Ps4AppEvent;

  typedef void (*Ps4AppEventCallback)(Ps4AppEvent *event, void *user);

  typedef struct ps4app_session_t
  {
    struct
    {
      struct addrinfo *host_addrinfos;
      struct addrinfo *host_addrinfo_selected;
      char hostname[128];
      char *regist_key;
      char *ostype;
      char auth[PS4APP_KEY_BYTES];
      uint8_t morning[PS4APP_KEY_BYTES];
      uint8_t did[PS4APP_RP_DID_SIZE];
    } connect_info;

    uint8_t nonce[PS4APP_KEY_BYTES];
    Ps4AppRPCrypt rpcrypt;

    Ps4AppQuitReason quit_reason;

    Ps4AppEventCallback event_cb;
    void *event_cb_user;

    Ps4AppThread session_thread;
    Ps4AppCtrl ctrl;

    Ps4AppLog log;
  } Ps4AppSession;

  PS4APP_EXPORT Ps4AppErrorCode
  ps4app_session_init(Ps4AppSession *session, Ps4AppConnectInfo *connect_info);
  PS4APP_EXPORT void ps4app_session_finish(Ps4AppSession *session);
  PS4APP_EXPORT Ps4AppErrorCode ps4app_session_start(Ps4AppSession *session);
  PS4APP_EXPORT Ps4AppErrorCode ps4app_session_join(Ps4AppSession *session);

  static inline void ps4app_session_set_event_cb(Ps4AppSession *session,
                                                 Ps4AppEventCallback cb, void *user)
  {
    session->event_cb = cb;
    session->event_cb_user = user;
  }

static inline void ps4app_session_set_quit_reason(Ps4AppSession *session, Ps4AppQuitReason reason){
  if (session->quit_reason != PS4APP_QUIT_REASON_NONE)
  return;
  session->quit_reason = reason;
}

#ifdef __cplusplus
}
#endif

#endif