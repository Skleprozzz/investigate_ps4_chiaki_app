#ifndef PS4APP_BASE64_H
#define PS4APP_BASE64_H

#include "common.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

PS4APP_EXPORT Ps4AppErrorCode ps4app_base64_decode(const char *in, size_t in_size, uint8_t *out, size_t *out_size);

#ifdef __cplusplus
}
#endif

#endif // App_BASE64_H
