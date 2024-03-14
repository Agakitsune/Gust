
#include <time.h>
#include <stddef.h>

#include "hash/hash.h"

// random seed
uint64_t SEED = 0x50a3618cb8077b56;

uint64_t murmur64(const uint64_t *key, uint64_t len, uint64_t seed) {
    const uint64_t m = 0xc6a4a7935bd1e995;
    const int r = 47;

    uint64_t h = seed ^ (len << 3); // multiply by 8

    for (uint64_t i = 0; i < len; ++i) {
        uint64_t k = key[i];

        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
    }

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
}

uint64_t murmur64str(const char *key, uint64_t len, uint64_t seed) {
    const uint64_t m = 0xc6a4a7935bd1e995;
    const int r = 47;

    uint64_t h = seed ^ (len);

    const uint64_t *data = (const uint64_t *)key;
    const uint64_t *end = data + (len >> 3);

    while (data != end) {
        uint64_t k = *data++;

        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
    }

    const unsigned char *data2 = (const unsigned char *)data;

    for (uint64_t i = 0; i < (len & 7); i++) {
        h ^= (uint64_t)data2[i] << (i << 8);
    }
    h *= m;

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
}
