#include <ps4app/log.h>

#include <stdio.h>
#include <stdarg.h>

void ps4app_log(Ps4AppLog *log, Ps4AppLogLevel level, const char *fmt, ...)
{

    va_list args;
    va_start(args, fmt);

    char c;
    switch (level)
    {
    case PS4APP_LOG_DEBUG:
        c = 'D';
        break;
    case PS4APP_LOG_INFO:
        c = 'I';
        break;
    case PS4APP_LOG_WARNING:
        c = 'W';
        break;
    case PS4APP_LOG_ERROR:
        c = 'E';
        break;
    default:
        c = '?';
        break;
    }
}