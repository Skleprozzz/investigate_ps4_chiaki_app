#ifndef PS4APP_LOG_H
#define PS4APP_LOG_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        PS4APP_LOG_DEBUG,
        PS4APP_LOG_INFO,
        PS4APP_LOG_WARNING,
        PS4APP_LOG_ERROR
    } Ps4AppLogLevel;

    typedef struct ps4app_log_t
    {

    } Ps4AppLog;

    void ps4app_log(Ps4AppLog *log, Ps4AppLogLevel level, const char *fmt, ...);
    void ps4app_log_hexdump(Ps4AppLog *log, Ps4AppLogLevel level, const uint8_t *buf, size_t buf_size);

#define PS4APP_LOGD(log, ...)                             \
    do                                                    \
    {                                                     \
        ps4app_log((log), PS4APP_LOG_DEBUG, __VA_ARGS__); \
    } while (0);
#define PS4APP_LOGI(log, ...)                            \
    do                                                   \
    {                                                    \
        ps4app_log((log), PS4APP_LOG_INFO, __VA_ARGS__); \
    } while (0);
#define PS4APP_LOGW(log, ...)                               \
    do                                                      \
    {                                                       \
        ps4app_log((log), PS4APP_LOG_WARNING, __VA_ARGS__); \
    } while (0);
#define PS4APP_LOGE(log, ...)                             \
    do                                                    \
    {                                                     \
        ps4app_log((log), PS4APP_LOG_ERROR, __VA_ARGS__); \
    } while (0);
#ifdef __cplusplus
}

#endif
#endif