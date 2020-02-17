#ifndef PS4APP_RPCRPT_H
#define PS4APP_RPCRYPT_H

#include "common.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define PS4APP_KEY_BYTES 0x10

    typedef struct ps4app_rpcrypt_t
    {
        uint8_t bright[PS4APP_KEY_BYTES];
        uint8_t ambassador[PS4APP_KEY_BYTES];
        struct hmac_ctx_st *hmac_ctx;
    } Ps4AppRPCrypt;

    PS4APP_EXPORT void ps4prcrypt_bright_ambassador(uint8_t *bright, uint8_t *ambassabor, const uint8_t *nonce, const uint8_t *morning);
    PS4APP_EXPORT void ps4app_rpcrypt_init(Ps4AppRPCrypt *rpcrypt, const uint8_t *nonce, const uint8_t *morning);
    PS4APP_EXPORT Ps4AppErrorCode ps4app_rpcrypt_generate_iv(Ps4AppRPCrypt *rpcrypt, uint8_t *iv, uint64_t counter);

#ifdef __cplusplus
}
#endif
#endif