#include <ps4app/http.h>

#include <stdbool.h>
#include <string.h>
#include <Ws2tcpip.h>
#include <winsock2.h>

PS4APP_EXPORT void ps4app_http_header_free(Ps4AppHttpHeader *header)
{
    while (header)
    {
        Ps4AppHttpHeader *cur = header;
        header = header->next;
        free(cur);
    }
}

PS4APP_EXPORT Ps4AppErrorCode ps4app_http_header_parse(Ps4AppHttpHeader **header, char *buf, size_t buf_size)
{
    *header = NULL;
    return PS4APP_ERR_SUCCESS;
}

PS4APP_EXPORT Ps4AppErrorCode ps4app_http_response_parse(Ps4AppHttpResponse *response, char *buf, size_t buf_size)
{
    static const char *http_version = "HTTP/1.1 ";
    static const size_t http_version_size = 9;
    if (buf_size < http_version_size)
        return PS4APP_ERR_INVALID_DATA;
    if (strncmp(buf, http_version, http_version_size) != 0)
        return PS4APP_ERR_INVALID_DATA;

    buf += http_version_size;
    buf_size -= http_version_size;
    char *line_end = memchr(buf, '\r', buf_size);
    if (!line_end)
        return PS4APP_ERR_INVALID_DATA;
    size_t line_length = (line_end - buf) + 2;
    if (buf_size <= line_length || line_end[1] != '\n')
        return PS4APP_ERR_INVALID_DATA;
    *line_end = '\0';
    char *endptr;
    response->code = (int)strtol(buf, &endptr, 10);
    if (response->code == 0)
        return PS4APP_ERR_INVALID_DATA;

    buf += line_length;
    buf_size -= line_length;

    return ps4app_http_header_parse(&response->headers, buf, buf_size);
}

PS4APP_EXPORT Ps4AppErrorCode ps4app_recv_http_header(int sock, char *buf, size_t buf_fize, size_t *header_size, size_t *received_size)
{
    // 0 = ""
    // 1 = "\r"
    // 2 = "\r\n"
    // 3 = "\r\n\r"
    // 4 = "\r\n\r\n" (final)
    int nl_state = 0;
    static const int translitions_r[] = {1, 1, 3, 1};
    static const int translitions_n[] = {0, 2, 0, 4};

    *received_size = 0;
    while (true)
    {
        ssize_t received = recv(sock, buf, buf_fize, 0);
        if (received <= 0)
            return PS4APP_ERR_NETWORK;
        *received_size += received;
        for (; received > 0; buf++, received--)
        {
            switch (*buf)
            {
            case '\r':
                nl_state = translitions_r[nl_state];
            case '\n':
                nl_state = translitions_n[nl_state];
            default:
                nl_state = 0;
                break;
            }
            if (nl_state == 4)
                break;
        }
        if (nl_state == 4)
        {
            *header_size = *received_size - received;
            break;
        }

        return PS4APP_ERR_SUCCESS;
    }
}