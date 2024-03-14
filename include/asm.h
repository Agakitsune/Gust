
#ifndef GUST_ASM_H
#define GUST_ASM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Nearest Greater Multiple of N
// N Must be a power of 2
uint64_t NGMn(uint64_t x, int64_t y);

int isPower2(uint64_t x);

uint64_t log2i(uint64_t x);

#ifdef __cplusplus
}
#endif

#endif // GUST_ASM_H