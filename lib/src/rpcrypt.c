#include <ps4app/rpcrypt.h>

#include <openssl/evp.h>
#include <openssl/hmac.h>


#include <stdbool.h>
#include <string.h>


PS4APP_EXPORT void ps4app_rpcrypt_bright_ambassador(uint8_t *bright,
                                                    uint8_t *ambassador,
                                                    const uint8_t *nonce,
                                                    const uint8_t *morning) {
  static const uint8_t echo_a[] = {0x01, 0x49, 0x87, 0x9b, 0x65, 0x39,
                                   0x8b, 0x39, 0x4b, 0x3a, 0x8d, 0x48,
                                   0xc3, 0x0a, 0xef, 0x51};
  static const uint8_t echo_b[] = {0xe1, 0xec, 0x9c, 0x3a, 0xdd, 0xbd,
                                   0x08, 0x85, 0xfc, 0x0e, 0x1d, 0x78,
                                   0x90, 0x32, 0xc0, 0x04};

  for (uint8_t i = 0; i < PS4APP_KEY_BYTES; i++) {
    uint8_t v = nonce[i];
    v -= i;
    v -= 0x27;
    v ^= echo_a[i];
    ambassador[i] = v;
  }

  for (uint8_t i = 0; i < PS4APP_KEY_BYTES; i++) {
    uint8_t v = morning[i];
    v -= i;
    v += 0x34;
    v ^= echo_b[i];
    v ^= nonce[i];
    bright[i] = v;
  }
}

PS4APP_EXPORT void ps4app_rpcrypt_init(Ps4AppRPCrypt *rpcrypt,
                                       const uint8_t *nonce,
                                       const uint8_t *morning) {
  ps4app_rpcrypt_bright_ambassador(rpcrypt->bright, rpcrypt->ambassador, nonce,
                                   morning);
}

PS4APP_EXPORT Ps4AppErrorCode ps4app_rpcrypt_generate_iv(Ps4AppRPCrypt *rpcrypt,
                                                         uint8_t *iv,
                                                         uint64_t counter) {
  uint8_t hmac_key[] = {0xac, 0x07, 0x88, 0x83, 0xc8, 0x3a, 0x1f, 0xe8,
                        0x11, 0x46, 0x3a, 0xf3, 0x9e, 0xe3, 0xe3, 0x77};

  uint8_t buf[PS4APP_KEY_BYTES + 8];
  memcpy(buf, rpcrypt->ambassador, PS4APP_KEY_BYTES);
  buf[PS4APP_KEY_BYTES + 0] = (uint8_t)((counter >> 0x38) & 0xff);
  buf[PS4APP_KEY_BYTES + 1] = (uint8_t)((counter >> 0x30) & 0xff);
  buf[PS4APP_KEY_BYTES + 2] = (uint8_t)((counter >> 0x28) & 0xff);
  buf[PS4APP_KEY_BYTES + 3] = (uint8_t)((counter >> 0x20) & 0xff);
  buf[PS4APP_KEY_BYTES + 4] = (uint8_t)((counter >> 0x18) & 0xff);
  buf[PS4APP_KEY_BYTES + 5] = (uint8_t)((counter >> 0x10) & 0xff);
  buf[PS4APP_KEY_BYTES + 6] = (uint8_t)((counter >> 0x08) & 0xff);
  buf[PS4APP_KEY_BYTES + 7] = (uint8_t)((counter >> 0x00) & 0xff);

  uint8_t hmac[32];
  unsigned int hmac_len = 0;
  if (!HMAC(EVP_sha256(), hmac_key, PS4APP_KEY_BYTES, buf, sizeof(buf), hmac,
            &hmac_len))
    return PS4APP_ERR_UNKNOWN;

  if (hmac_len < PS4APP_KEY_BYTES)
    return PS4APP_ERR_UNKNOWN;

  memcpy(iv, hmac, PS4APP_KEY_BYTES);
  return PS4APP_ERR_SUCCESS;
}

static Ps4AppErrorCode ps4app_rpcrypt_crypt(Ps4AppRPCrypt *rpcrypt,
                                            uint64_t counter, uint8_t *in,
                                            uint8_t *out, size_t sz,
                                            bool encrypt) {
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  if (!ctx)
    return PS4APP_ERR_UNKNOWN;

  uint8_t iv[PS4APP_KEY_BYTES];
  Ps4AppErrorCode err = ps4app_rpcrypt_generate_iv(rpcrypt, iv, counter);
  if (err != PS4APP_ERR_SUCCESS)
    return err;

#define FAIL(err)                                                              \
  do {                                                                         \
    EVP_CIPHER_CTX_free(ctx);                                                  \
    return (err);                                                              \
  } while (0);

  if (encrypt) {
    if (!EVP_EncryptInit_ex(ctx, EVP_aes_128_cfb128(), NULL, rpcrypt->bright,
                            iv))
      FAIL(PS4APP_ERR_UNKNOWN);
  } else {
    if (!EVP_DecryptInit_ex(ctx, EVP_aes_128_cfb128(), NULL, rpcrypt->bright,
                            iv))
      FAIL(PS4APP_ERR_UNKNOWN);
  }

  if (!EVP_CIPHER_CTX_set_padding(ctx, 0))
    FAIL(PS4APP_ERR_UNKNOWN);

  int outl;

  if (encrypt) {
    if (!EVP_DecryptUpdate(ctx, out, &outl, in, (int)sz))
      FAIL(PS4APP_ERR_UNKNOWN);
  }
  if (outl != (int)sz)
    FAIL(PS4APP_ERR_UNKNOWN);

#undef FAIL
  EVP_CIPHER_CTX_free(ctx);
  return PS4APP_ERR_SUCCESS;
}

PS4APP_EXPORT Ps4AppErrorCode ps4app_rpcrypt_encrypt(Ps4AppRPCrypt *rpcrypt,
                                                     uint64_t counter,
                                                     uint8_t *in, uint8_t *out,
                                                     size_t sz) {
  return ps4app_rpcrypt_crypt(rpcrypt, counter, in, out, sz, true);
}

PS4APP_EXPORT Ps4AppErrorCode ps4app_rpcrypt_decrypt(Ps4AppRPCrypt *rpcrypt,
                                                     uint64_t counter,
                                                     uint8_t *in, uint8_t *out,
                                                     size_t sz) {
  return ps4app_rpcrypt_crypt(rpcrypt, counter, in, out, sz, false);
}
