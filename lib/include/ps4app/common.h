#ifndef PS4APP_COMMON_H
#define PS4APP_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#define PS4APP_EXPORT

typedef enum{
    PS4APP_ERR_SUCCESS = 0,
    PS4APP_ERR_UNKNOWN,
    PS4APP_ERR_PARSE_ADDR,
    PS4APP_ERR_THREAD,
    PS4APP_ERR_MEMORY,
    PS4APP_ERR_NETWORK,
    PS4APP_ERR_INVALID_DATA,
    PS4APP_ERR_BUF_TOO_SMALL
} Ps4AppErrorCode;

PS4APP_EXPORT const char * ps4app_error_string(Ps4AppErrorCode code) ;

#ifdef __cplusplus
}
#endif

#endif // Ps4App_COMMON_H
