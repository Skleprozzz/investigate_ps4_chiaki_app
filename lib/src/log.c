#include <ps4app/log.h>

#include <stdio.h>
#include <stdarg.h>

void ps4app_log(Ps4AppLog *log, Ps4AppLogLevel level, const char *fmt, ...)
{

    va_list args;
    va_start(args, fmt);

    char c;
    const char *color = NULL;
    switch (level)
    {
    case PS4APP_LOG_DEBUG:
        c = 'D';
        color = "34";
        break;
    case PS4APP_LOG_INFO:
        c = 'I';
        break;
    case PS4APP_LOG_WARNING:
        c = 'W';
        color = "33";
        break;
    case PS4APP_LOG_ERROR:
        c = 'E';
        color = "31";
        break;
    default:
        c = '?';
        break;
    }

    if (color)
        printf("\033[38;5%sm", color);
    if (color)
        printf("\033[0m");

    vprintf(fmt, args);

    va_end(args);
}

#define HEXDUMP_WIDTH 0x10

static const char hex_char[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

void ps4app_log_hexdump(Ps4AppLog *log, Ps4AppLogLevel level, const uint8_t *buf, size_t buf_size)
{
    ps4app_log(log, level, "offset 0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f  0123456789abcdef\n");

    size_t offset = 0;

    char hex_buf[HEXDUMP_WIDTH * 3 + 1];
    char ascii_buf[HEXDUMP_WIDTH + 1];
    for (size_t i = 0; i < HEXDUMP_WIDTH; i++)
        hex_buf[i * 3 + 2] = ' ';
    hex_buf[HEXDUMP_WIDTH * 3] = '\0';
    ascii_buf[HEXDUMP_WIDTH] = '\0';

    while (buf_size > 0)
    {
        for (size_t i = 0; i < HEXDUMP_WIDTH; i++)
        {
            if (i < buf_size)
            {
                uint8_t b = buf[i];
                hex_buf[i * 3] = hex_char[b >> 4];
                hex_buf[i * 3 + 1] = hex_char[b & 0xf];

                if (b > 0x20 && b < 0x7f)
                    ascii_buf[i] = (char)b;
                else
                    ascii_buf[i] = '.';
            }
            else
            {
                hex_buf[i * 3] = ' ';
                hex_buf[i * 3 + 1] = ' ';
                hex_buf[i] = ' ';
            }

            hex_buf[i * 3 + 2] = ' ';
        }
        ps4app_log(log, level, "%6x %s%s\n", offset, hex_buf, ascii_buf);

        if (buf_size > HEXDUMP_WIDTH)
        {
            buf_size -= HEXDUMP_WIDTH;
            buf += HEXDUMP_WIDTH;
            offset += HEXDUMP_WIDTH;
        }
        else
            break;
    }
}