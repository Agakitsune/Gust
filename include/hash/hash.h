
#ifndef GUST_HASH_H
#define GUST_HASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern uint64_t SEED;

uint64_t murmur64(const uint64_t *key, uint64_t len, uint64_t seed);
uint64_t murmur64str(const char *key, uint64_t len, uint64_t seed);

#define murmur64seedless(key, len) murmur64((key), (len), SEED)
#define murmur64strseedless(key, len) murmur64str((key), (len), SEED)

#ifdef __cplusplus
}
#endif

#endif // GUST_HASH_H
