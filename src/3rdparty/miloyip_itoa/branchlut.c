/*
 * Original code from Milo Yip
 *     https://github.com/miloyip/itoa-benchmark/src/branchlut.cpp
 *     commit #27c6059
 *
 * Adapted to be C99 compatible
 */

#include "branchlut.h"

static const char gDigitsLut[200] = {
    '0','0','0','1','0','2','0','3','0','4','0','5','0','6','0','7','0','8','0','9',
    '1','0','1','1','1','2','1','3','1','4','1','5','1','6','1','7','1','8','1','9',
    '2','0','2','1','2','2','2','3','2','4','2','5','2','6','2','7','2','8','2','9',
    '3','0','3','1','3','2','3','3','3','4','3','5','3','6','3','7','3','8','3','9',
    '4','0','4','1','4','2','4','3','4','4','4','5','4','6','4','7','4','8','4','9',
    '5','0','5','1','5','2','5','3','5','4','5','5','5','6','5','7','5','8','5','9',
    '6','0','6','1','6','2','6','3','6','4','6','5','6','6','6','7','6','8','6','9',
    '7','0','7','1','7','2','7','3','7','4','7','5','7','6','7','7','7','8','7','9',
    '8','0','8','1','8','2','8','3','8','4','8','5','8','6','8','7','8','8','8','9',
    '9','0','9','1','9','2','9','3','9','4','9','5','9','6','9','7','9','8','9','9'
};

/*
 * Branching for different cases (forward)
 * Use lookup table of two digits
 */

char* u32toa_branchlut(uint32_t value, char* buffer) {
    if (value < 10000) {
        const uint32_t d1 = (value / 100) << 1;
        const uint32_t d2 = (value % 100) << 1;

        if (value >= 1000)
            *buffer++ = gDigitsLut[d1];
        if (value >= 100)
            *buffer++ = gDigitsLut[d1 + 1];
        if (value >= 10)
            *buffer++ = gDigitsLut[d2];
        *buffer++ = gDigitsLut[d2 + 1];
    }
    else if (value < 100000000) {
        /* value = bbbbcccc */
        const uint32_t b = value / 10000;
        const uint32_t c = value % 10000;

        const uint32_t d1 = (b / 100) << 1;
        const uint32_t d2 = (b % 100) << 1;

        const uint32_t d3 = (c / 100) << 1;
        const uint32_t d4 = (c % 100) << 1;

        if (value >= 10000000)
            *buffer++ = gDigitsLut[d1];
        if (value >= 1000000)
            *buffer++ = gDigitsLut[d1 + 1];
        if (value >= 100000)
            *buffer++ = gDigitsLut[d2];
        *buffer++ = gDigitsLut[d2 + 1];

        *buffer++ = gDigitsLut[d3];
        *buffer++ = gDigitsLut[d3 + 1];
        *buffer++ = gDigitsLut[d4];
        *buffer++ = gDigitsLut[d4 + 1];
    }
    else {
        /* value = aabbbbcccc in decimal */

        const uint32_t a = value / 100000000; /* 1 to 42 */
        value %= 100000000;

        if (a >= 10) {
            const unsigned i = a << 1;
            *buffer++ = gDigitsLut[i];
            *buffer++ = gDigitsLut[i + 1];
        }
        else
            *buffer++ = '0' + (char)a;

        {
            const uint32_t b = value / 10000; /* 0 to 9999 */
            const uint32_t c = value % 10000; /* 0 to 9999 */

            const uint32_t d1 = (b / 100) << 1;
            const uint32_t d2 = (b % 100) << 1;

            const uint32_t d3 = (c / 100) << 1;
            const uint32_t d4 = (c % 100) << 1;

            *buffer++ = gDigitsLut[d1];
            *buffer++ = gDigitsLut[d1 + 1];
            *buffer++ = gDigitsLut[d2];
            *buffer++ = gDigitsLut[d2 + 1];
            *buffer++ = gDigitsLut[d3];
            *buffer++ = gDigitsLut[d3 + 1];
            *buffer++ = gDigitsLut[d4];
            *buffer++ = gDigitsLut[d4 + 1];
        }
    }
    /**buffer++ = '\0';*/
    return buffer;
}

char* i32toa_branchlut(int32_t value, char* buffer) {
    uint32_t u = (uint32_t)value;
    if (value < 0) {
        *buffer++ = '-';
        u = ~u + 1;
    }

    return u32toa_branchlut(u, buffer);
}

#ifdef GMIO_HAVE_INT64_TYPE
/* In case the compiler does not support ULL literals(C99) */
#define GMIO_UINT64_C(high32, low32) (((uint64_t)(high32) << 32) | (low32))
static const uint64_t one_e16 = GMIO_UINT64_C(0x002386f2, 0x6fc10000);
static const uint64_t one_e15 = GMIO_UINT64_C(0x00038d7e, 0xa4c68000);
static const uint64_t one_e14 = GMIO_UINT64_C(0x00005af3, 0x107a4000);
static const uint64_t one_e13 = GMIO_UINT64_C(0x00000918, 0x4e72a000);
static const uint64_t one_e12 = GMIO_UINT64_C(0x000000e8, 0xd4a51000);
static const uint64_t one_e11 = GMIO_UINT64_C(0x00000017, 0x4876e800);
static const uint64_t one_e10 = GMIO_UINT64_C(0x00000002, 0x540be400);

char* u64toa_branchlut(uint64_t value, char* buffer) {
    if (value < 100000000) {
        uint32_t v = (uint32_t)value;
        if (v < 10000) {
            const uint32_t d1 = (v / 100) << 1;
            const uint32_t d2 = (v % 100) << 1;

            if (v >= 1000)
                *buffer++ = gDigitsLut[d1];
            if (v >= 100)
                *buffer++ = gDigitsLut[d1 + 1];
            if (v >= 10)
                *buffer++ = gDigitsLut[d2];
            *buffer++ = gDigitsLut[d2 + 1];
        }
        else {
            /* value = bbbbcccc */
            const uint32_t b = v / 10000;
            const uint32_t c = v % 10000;

            const uint32_t d1 = (b / 100) << 1;
            const uint32_t d2 = (b % 100) << 1;

            const uint32_t d3 = (c / 100) << 1;
            const uint32_t d4 = (c % 100) << 1;

            if (value >= 10000000)
                *buffer++ = gDigitsLut[d1];
            if (value >= 1000000)
                *buffer++ = gDigitsLut[d1 + 1];
            if (value >= 100000)
                *buffer++ = gDigitsLut[d2];
            *buffer++ = gDigitsLut[d2 + 1];

            *buffer++ = gDigitsLut[d3];
            *buffer++ = gDigitsLut[d3 + 1];
            *buffer++ = gDigitsLut[d4];
            *buffer++ = gDigitsLut[d4 + 1];
        }
    }
    else if (value < one_e16) {
        const uint32_t v0 = (uint32_t)(value / 100000000);
        const uint32_t v1 = (uint32_t)(value % 100000000);

        const uint32_t b0 = v0 / 10000;
        const uint32_t c0 = v0 % 10000;

        const uint32_t d1 = (b0 / 100) << 1;
        const uint32_t d2 = (b0 % 100) << 1;

        const uint32_t d3 = (c0 / 100) << 1;
        const uint32_t d4 = (c0 % 100) << 1;

        const uint32_t b1 = v1 / 10000;
        const uint32_t c1 = v1 % 10000;

        const uint32_t d5 = (b1 / 100) << 1;
        const uint32_t d6 = (b1 % 100) << 1;

        const uint32_t d7 = (c1 / 100) << 1;
        const uint32_t d8 = (c1 % 100) << 1;

        if (value >= one_e15)
            *buffer++ = gDigitsLut[d1];
        if (value >= one_e14)
            *buffer++ = gDigitsLut[d1 + 1];
        if (value >= one_e13)
            *buffer++ = gDigitsLut[d2];
        if (value >= one_e12)
            *buffer++ = gDigitsLut[d2 + 1];
        if (value >= one_e11)
            *buffer++ = gDigitsLut[d3];
        if (value >= one_e10)
            *buffer++ = gDigitsLut[d3 + 1];
        if (value >= 1000000000)
            *buffer++ = gDigitsLut[d4];
        if (value >= 100000000)
            *buffer++ = gDigitsLut[d4 + 1];

        *buffer++ = gDigitsLut[d5];
        *buffer++ = gDigitsLut[d5 + 1];
        *buffer++ = gDigitsLut[d6];
        *buffer++ = gDigitsLut[d6 + 1];
        *buffer++ = gDigitsLut[d7];
        *buffer++ = gDigitsLut[d7 + 1];
        *buffer++ = gDigitsLut[d8];
        *buffer++ = gDigitsLut[d8 + 1];
    }
    else {
        const uint32_t a = (uint32_t)(value / one_e16); /* 1 to 1844 */
        value %= one_e16;

        if (a < 10)
            *buffer++ = '0' + (char)a;
        else if (a < 100) {
            const uint32_t i = a << 1;
            *buffer++ = gDigitsLut[i];
            *buffer++ = gDigitsLut[i + 1];
        }
        else if (a < 1000) {
            const uint32_t i = (a % 100) << 1;
            *buffer++ = '0' + (char)(a / 100);
            *buffer++ = gDigitsLut[i];
            *buffer++ = gDigitsLut[i + 1];
        }
        else {
            const uint32_t i = (a / 100) << 1;
            const uint32_t j = (a % 100) << 1;
            *buffer++ = gDigitsLut[i];
            *buffer++ = gDigitsLut[i + 1];
            *buffer++ = gDigitsLut[j];
            *buffer++ = gDigitsLut[j + 1];
        }

        {
            const uint32_t v0 = (uint32_t)(value / 100000000);
            const uint32_t v1 = (uint32_t)(value % 100000000);

            const uint32_t b0 = v0 / 10000;
            const uint32_t c0 = v0 % 10000;

            const uint32_t d1 = (b0 / 100) << 1;
            const uint32_t d2 = (b0 % 100) << 1;

            const uint32_t d3 = (c0 / 100) << 1;
            const uint32_t d4 = (c0 % 100) << 1;

            const uint32_t b1 = v1 / 10000;
            const uint32_t c1 = v1 % 10000;

            const uint32_t d5 = (b1 / 100) << 1;
            const uint32_t d6 = (b1 % 100) << 1;

            const uint32_t d7 = (c1 / 100) << 1;
            const uint32_t d8 = (c1 % 100) << 1;

            *buffer++ = gDigitsLut[d1];
            *buffer++ = gDigitsLut[d1 + 1];
            *buffer++ = gDigitsLut[d2];
            *buffer++ = gDigitsLut[d2 + 1];
            *buffer++ = gDigitsLut[d3];
            *buffer++ = gDigitsLut[d3 + 1];
            *buffer++ = gDigitsLut[d4];
            *buffer++ = gDigitsLut[d4 + 1];
            *buffer++ = gDigitsLut[d5];
            *buffer++ = gDigitsLut[d5 + 1];
            *buffer++ = gDigitsLut[d6];
            *buffer++ = gDigitsLut[d6 + 1];
            *buffer++ = gDigitsLut[d7];
            *buffer++ = gDigitsLut[d7 + 1];
            *buffer++ = gDigitsLut[d8];
            *buffer++ = gDigitsLut[d8 + 1];
        }
    }

    /**buffer++ = '\0';*/
    return buffer;
}

char* i64toa_branchlut(int64_t value, char* buffer) {
    uint64_t u = (uint64_t)value;
    if (value < 0) {
        *buffer++ = '-';
        u = ~u + 1;
    }

    return u64toa_branchlut(u, buffer);
}
#endif /* GMIO_HAVE_INT64_TYPE */
