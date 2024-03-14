
#include "asm.h"

inline uint64_t NGMn(uint64_t x, int64_t y) {
    return (x + (y - 1)) & -y;
}

inline int isPower2(uint64_t x) {
    return (x & (x - 1)) == 0;
}

inline uint64_t log2i(uint64_t x) {
    uint64_t y;
    asm (
        "\tbsr %1, %0\n"
        : "=r"(y)
        : "r"(x)
    );
    return y;
}
