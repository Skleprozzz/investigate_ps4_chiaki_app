#include <ps4app/common.h>

PS4APP_EXPORT const char *ps4app_error_string(Ps4AppErrorCode code)
{
    switch (code)
    {
    case PS4APP_ERR_SUCCESS:
        return "success";
    case PS4APP_ERR_PARSE_ADDR:
        return "Failed to parse host address";
    case PS4APP_ERR_THREAD:
        return "Thread error";
    case PS4APP_ERR_MEMORY:
        return "Memory error";
    case PS4APP_ERR_NETWORK:
        return "Network error";
    case PS4APP_ERR_INVALID_DATA:
        return "Invalid data";

    default:
        return "Unknown";
    }
}