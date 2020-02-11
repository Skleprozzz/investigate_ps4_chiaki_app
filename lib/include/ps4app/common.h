#ifndef PS4APP_COMMON_H
#define PS4APP_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#define PS4APP_EXPORT

typedef enum{
    PS4APP_ERR_SUCCESS = 0,
    PS4APP_ERR_PARSE_ADDR = 1,
    PS4APP_ERR_THREAD = 2,
    PS4APP_ERR_MEMORY = 3,
    PS4APP_ERR_NETWORK = 4,
    PS4APP_ERR_INVALID_DATA = 5,
    PS4APP_ERR_BUF_TOO_SMALL = 6
} Ps4AppErrorCode;

PS4APP_EXPORT const char * ps4app_error_string(Ps4AppErrorCode code) ;

#ifdef __cplusplus
}
#endif

#endif // Ps4App_COMMON_H
