#ifndef PS4APP_HTTP_H
#define PS4APP_HTTP_H

#include "common.h"

#include <stdlib.h>

#ifdef __cpluplus
extern "C"
{
#endif

    typedef struct ps4app_http_header_t
    {
        const char *key;
        const char *value;
        struct ps4app_http_header_t *next;

    } Ps4AppHttpHeader;

    typedef struct ps4app_http_response_t
    {
        int code;
        Ps4AppHttpHeader *headers;
    } Ps4AppHttpResponse;

    PS4APP_EXPORT void ps4app_http_header_free(Ps4AppHttpHeader *header);
    PS4APP_EXPORT Ps4AppErrorCode ps4app_http_header_parse(Ps4AppHttpHeader **header, char *buf, size_t buf_size);
    PS4APP_EXPORT void ps4app_http_response_finish(Ps4AppHttpResponse *response);
    PS4APP_EXPORT Ps4AppErrorCode ps4app_http_response_parse(Ps4AppHttpResponse *response, char *buf, size_t buf_size);
    PS4APP_EXPORT Ps4AppErrorCode ps4app_recv_http_header(int sock, char *buf, size_t buf_size, size_t *header_size, size_t *received_size);

#ifdef __cpluplus
}
#endif

#endif //PS4APP_HTTP_H