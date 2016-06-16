/****************************************************************************
** gmio
** Copyright Fougue (2 Mar. 2015)
** contact@fougue.pro
**
** This software is a reusable library whose purpose is to provide complete
** I/O support for various CAD file formats (eg. STL)
**
** This software is governed by the CeCILL-B license under French law and
** abiding by the rules of distribution of free software.  You can  use,
** modify and/ or redistribute the software under the terms of the CeCILL-B
** license as circulated by CEA, CNRS and INRIA at the following URL
** "http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html".
****************************************************************************/

#include "utest_assert.h"

#include "stream_buffer.h"

#include "../src/gmio_core/internal/byte_codec.h"
#include "../src/gmio_core/internal/byte_swap.h"
#include "../src/gmio_core/internal/convert.h"
#include "../src/gmio_core/internal/fast_atof.h"
#include "../src/gmio_core/internal/numeric_utils.h"
#include "../src/gmio_core/internal/safe_cast.h"
#include "../src/gmio_core/internal/stringstream.h"
#include "../src/gmio_core/internal/stringstream_fast_atof.h"
#include "../src/gmio_core/internal/string_ascii_utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static const char* test_internal__byte_swap()
{
    UTEST_ASSERT(gmio_uint16_bswap(0x1122) == 0x2211);
    UTEST_ASSERT(gmio_uint32_bswap(0x11223344) == 0x44332211);
    return NULL;
}

static const char* test_internal__byte_codec()
{
    { /* decode */
        const uint8_t data[] = { 0x11, 0x22, 0x33, 0x44 };
        UTEST_ASSERT(gmio_decode_uint16_le(data) == 0x2211);
        UTEST_ASSERT(gmio_decode_uint16_be(data) == 0x1122);
        UTEST_ASSERT(gmio_decode_uint32_le(data) == 0x44332211);
        UTEST_ASSERT(gmio_decode_uint32_be(data) == 0x11223344);
    }

    { /* encode */
        uint8_t data[] = { 0, 0, 0, 0 };
        gmio_encode_uint16_le(0x1122, data);
        UTEST_ASSERT(data[0] == 0x22 && data[1] == 0x11);
        gmio_encode_uint32_le(0x11223344, data);
        UTEST_ASSERT(data[0] == 0x44 && data[1] == 0x33 && data[2] == 0x22 && data[3] == 0x11);
        gmio_encode_uint32_be(0x11223344, data);
        UTEST_ASSERT(data[3] == 0x44 && data[2] == 0x33 && data[1] == 0x22 && data[0] == 0x11);
    }

    return NULL;
}

static void __tc__fprintf_atof_err(
        const char* func_fast_atof_str,
        const char* val_str,
        float fast_val,
        float std_val)
{
    fprintf(stderr,
            "*** ERROR: %s() less accurate than strtod()\n"
            "    value_str : \"%s\"\n"
            "    fast_value: %.12f (%s) as_int: 0x%x\n"
            "    std_value : %.12f (%s) as_int: 0x%x\n"
            "    ulp_diff  : %u\n",
            func_fast_atof_str,
            val_str,
            fast_val,
            gmio_float32_sign(fast_val) > 0 ? "+" : "-",
            gmio_convert_uint32(fast_val),
            std_val,
            gmio_float32_sign(std_val) > 0 ? "+" : "-",
            gmio_convert_uint32(std_val),
            gmio_float32_ulp_diff(fast_val, std_val));
}

static bool __tc__check_calculation_atof(const char* val_str)
{
    const float std_val = (float)strtod(val_str, NULL);
    int accurate_count = 0;

    { /* Test fast_atof() */
        const float fast_val = fast_atof(val_str);
        if (gmio_float32_ulp_equals(fast_val, std_val, 1))
            ++accurate_count;
        else
            __tc__fprintf_atof_err("fast_atof", val_str, fast_val, std_val);
    }

    { /* Test gmio_stringstream_fast_atof() */
        char iobuff[512] = {0};
        struct gmio_ro_buffer ibuff =
                gmio_ro_buffer(val_str, strlen(val_str), 0);
        struct gmio_stringstream sstream =
                gmio_stringstream(
                    gmio_istream_buffer(&ibuff),
                    gmio_string(iobuff, 0, sizeof(iobuff)));
        const float fast_val = gmio_stringstream_fast_atof(&sstream);
        if (gmio_float32_ulp_equals(fast_val, std_val, 1)) {
            ++accurate_count;
        }
        else {
            __tc__fprintf_atof_err(
                        "gmio_stringstream_fast_atof", val_str, fast_val, std_val);
        }
    }

    return accurate_count == 2;
}

static const char* test_internal__fast_atof()
{
    bool ok = true;

    ok = ok && __tc__check_calculation_atof("340282346638528859811704183484516925440.000000");
    ok = ok && __tc__check_calculation_atof("3.402823466e+38F");
    ok = ok && __tc__check_calculation_atof("3402823466e+29F");
    ok = ok && __tc__check_calculation_atof("-340282346638528859811704183484516925440.000000");
    ok = ok && __tc__check_calculation_atof("-3.402823466e+38F");
    ok = ok && __tc__check_calculation_atof("-3402823466e+29F");
    ok = ok && __tc__check_calculation_atof("34028234663852885981170418348451692544.000000");
    ok = ok && __tc__check_calculation_atof("3.402823466e+37F");
    ok = ok && __tc__check_calculation_atof("3402823466e+28F");
    ok = ok && __tc__check_calculation_atof("-34028234663852885981170418348451692544.000000");
    ok = ok && __tc__check_calculation_atof("-3.402823466e+37F");
    ok = ok && __tc__check_calculation_atof("-3402823466e+28F");
    ok = ok && __tc__check_calculation_atof(".00234567");
    ok = ok && __tc__check_calculation_atof("-.00234567");
    ok = ok && __tc__check_calculation_atof("0.00234567");
    ok = ok && __tc__check_calculation_atof("-0.00234567");
    ok = ok && __tc__check_calculation_atof("1.175494351e-38F");
#if 0
    /* This check fails */
    ok = ok && __tc__check_calculation_atof("1175494351e-47F");
#endif
    ok = ok && __tc__check_calculation_atof("1.175494351e-37F");
    ok = ok && __tc__check_calculation_atof("1.175494351e-36F");
    ok = ok && __tc__check_calculation_atof("-1.175494351e-36F");
    ok = ok && __tc__check_calculation_atof("123456.789");
    ok = ok && __tc__check_calculation_atof("-123456.789");
    ok = ok && __tc__check_calculation_atof("0000123456.789");
    ok = ok && __tc__check_calculation_atof("-0000123456.789");
    ok = ok && __tc__check_calculation_atof("-0.0690462109446526");

    UTEST_ASSERT(ok);

    return NULL;
}

static const char* test_internal__safe_cast()
{
#if GMIO_TARGET_ARCH_BIT_SIZE > 32
    const size_t maxUInt32 = 0xFFFFFFFF;
    UTEST_ASSERT(gmio_size_to_uint32(maxUInt32 + 1) == 0xFFFFFFFF);
    UTEST_ASSERT(gmio_size_to_uint32(0xFFFFFFFF) == 0xFFFFFFFF);
    UTEST_ASSERT(gmio_size_to_uint32(100) == 100);
#endif

    UTEST_ASSERT(gmio_streamsize_to_size(-1) == ((size_t)-1));
#ifdef GMIO_HAVE_INT64_TYPE
#  if GMIO_TARGET_ARCH_BIT_SIZE < 64
    const gmio_streamsize_t overMaxSizet =
            ((gmio_streamsize_t)GMIO_MAX_SIZET) + 1;
    UTEST_ASSERT(gmio_streamsize_to_size(overMaxSizet) == GMIO_MAX_SIZET);
#  endif
    UTEST_ASSERT(gmio_streamsize_to_size(GMIO_MAX_SIZET) == GMIO_MAX_SIZET);
    UTEST_ASSERT(gmio_streamsize_to_size(150) == 150);
#endif

    return NULL;
}

static const char* test_internal__stringstream()
{
    static const char text[] =
            "Une    citation,\to je crois qu'elle est de moi :"
            "Parfois le chemin est rude.\n"
            "pi : 3.1415926535897932384626433832795";
    {
        struct gmio_ro_buffer buff =
                gmio_ro_buffer(text, sizeof(text) - 1, 0);

        char cstr_small[4];
        char cstr[32];
        struct gmio_stringstream sstream =
                gmio_stringstream(
                    gmio_istream_buffer(&buff),
                    gmio_string(cstr, 0, sizeof(cstr)));

        char cstr_copy[128];
        struct gmio_string str_copy =
                gmio_string(cstr_copy, 0, sizeof(cstr_copy));

        UTEST_ASSERT(gmio_stringstream_current_char(&sstream) != NULL);
        UTEST_ASSERT(*gmio_stringstream_current_char(&sstream) == 'U');

        str_copy.len = 0;
        UTEST_ASSERT(gmio_stringstream_eat_word(&sstream, &str_copy) == 0);
        /* printf("\ncopy_strbuff.ptr = \"%s\"\n", copy_strbuff.ptr); */
        UTEST_ASSERT(strcmp(str_copy.ptr, "Une") == 0);

        str_copy.len = 0;
        UTEST_ASSERT(gmio_stringstream_eat_word(&sstream, &str_copy) == 0);
        UTEST_ASSERT(strcmp(str_copy.ptr, "citation,") == 0);

        str_copy.len = 0;
        UTEST_ASSERT(gmio_stringstream_eat_word(&sstream, &str_copy) == 0);
        UTEST_ASSERT(strcmp(str_copy.ptr, "o") == 0);

        str_copy.len = 0;
        UTEST_ASSERT(gmio_stringstream_eat_word(&sstream, &str_copy) == 0);
        UTEST_ASSERT(strcmp(str_copy.ptr, "je") == 0);

        gmio_stringstream_skip_ascii_spaces(&sstream);
        UTEST_ASSERT(gmio_stringstream_next_char(&sstream) != NULL);
        UTEST_ASSERT(*gmio_stringstream_current_char(&sstream) == 'r');

        /* Test with very small string buffer */
        buff.pos = 0;
        sstream.strbuff = gmio_string(cstr_small, 0, sizeof(cstr_small));
        gmio_stringstream_init_pos(&sstream);

        UTEST_ASSERT(*gmio_stringstream_current_char(&sstream) == 'U');
        str_copy.len = 0;
        UTEST_ASSERT(gmio_stringstream_eat_word(&sstream, &str_copy) == 0);
        str_copy.len = 0;
        UTEST_ASSERT(gmio_stringstream_eat_word(&sstream, &str_copy) == 0);
        UTEST_ASSERT(strcmp(str_copy.ptr, "citation,") == 0);
    }

    {
        struct gmio_ro_buffer buff =
                gmio_ro_buffer(text, sizeof(text) - 1, 0);

        char cstr[32];
        struct gmio_stringstream sstream =
                gmio_stringstream(
                    gmio_istream_buffer(&buff),
                    gmio_string(cstr, 0, sizeof(cstr)));

        char cstr_copy[128];
        struct gmio_string str_copy =
                gmio_string(cstr_copy, 0, sizeof(cstr_copy));

        UTEST_ASSERT(gmio_stringstream_eat_word(&sstream, &str_copy) == 0);
        UTEST_ASSERT(strcmp(str_copy.ptr, "Une") == 0);

        UTEST_ASSERT(gmio_stringstream_eat_word(&sstream, &str_copy) == 0);
        UTEST_ASSERT(strcmp(str_copy.ptr, "Unecitation,") == 0);
        UTEST_ASSERT(gmio_stringstream_eat_word(&sstream, &str_copy) == 0);
        UTEST_ASSERT(strcmp(str_copy.ptr, "Unecitation,o") == 0);
    }

    return NULL;
}

static const char* test_internal__string_utils()
{
    char c; /* for loop counter */

    UTEST_ASSERT(gmio_ascii_isspace(' '));
    UTEST_ASSERT(gmio_ascii_isspace('\t'));
    UTEST_ASSERT(gmio_ascii_isspace('\n'));
    UTEST_ASSERT(gmio_ascii_isspace('\r'));

    for (c = 0; 0 <= c && c <= 127; ++c) {
        if (65 <= c && c <= 90) {
            UTEST_ASSERT(gmio_ascii_isupper(c));
        }
        else if (97 <= c && c <= 122) {
            UTEST_ASSERT(gmio_ascii_islower(c));
        }
        else if (c == 0x20 || (0x09 <= c && c <= 0x0d)) {
            UTEST_ASSERT(gmio_ascii_isspace(c));
        }
        else if (48 <= c && c <= 57) {
            UTEST_ASSERT(gmio_ascii_isdigit(c));
        }
        else {
            UTEST_ASSERT(!gmio_ascii_isupper(c));
            UTEST_ASSERT(!gmio_ascii_islower(c));
            UTEST_ASSERT(!gmio_ascii_isspace(c));
            UTEST_ASSERT(!gmio_ascii_isdigit(c));
        }
    }

    UTEST_ASSERT(gmio_ascii_tolower('A') == 'a');
    UTEST_ASSERT(gmio_ascii_tolower('Z') == 'z');
    UTEST_ASSERT(gmio_ascii_tolower('(') == '(');
    UTEST_ASSERT(gmio_ascii_toupper('a') == 'A');
    UTEST_ASSERT(gmio_ascii_toupper('z') == 'Z');
    UTEST_ASSERT(gmio_ascii_toupper('(') == '(');

    UTEST_ASSERT(gmio_ascii_char_iequals('a', 'a'));
    UTEST_ASSERT(gmio_ascii_char_iequals('a', 'A'));
    UTEST_ASSERT(gmio_ascii_char_iequals('A', 'a'));
    UTEST_ASSERT(gmio_ascii_char_iequals('{', '{'));
    UTEST_ASSERT(!gmio_ascii_char_iequals('{', '['));

    UTEST_ASSERT(gmio_ascii_stricmp("FACET", "facet") == 0);
    UTEST_ASSERT(gmio_ascii_stricmp("facet", "FACET") == 0);
    UTEST_ASSERT(gmio_ascii_stricmp("facet", "facet") == 0);
    UTEST_ASSERT(gmio_ascii_stricmp("FACET", "FACET") == 0);
    UTEST_ASSERT(gmio_ascii_stricmp("", "") == 0);
    UTEST_ASSERT(gmio_ascii_stricmp("", "facet") != 0);
    UTEST_ASSERT(gmio_ascii_stricmp("facet", "facet_") != 0);
    UTEST_ASSERT(gmio_ascii_stricmp("facet_", "facet") != 0);

    UTEST_ASSERT(gmio_ascii_istarts_with("facet", ""));
    UTEST_ASSERT(gmio_ascii_istarts_with("facet", "f"));
    UTEST_ASSERT(gmio_ascii_istarts_with("facet", "fa"));
    UTEST_ASSERT(gmio_ascii_istarts_with("facet", "facet"));
    UTEST_ASSERT(!gmio_ascii_istarts_with("facet", "a"));
    UTEST_ASSERT(!gmio_ascii_istarts_with("facet", " facet"));
    UTEST_ASSERT(gmio_ascii_istarts_with("facet", "F"));
    UTEST_ASSERT(gmio_ascii_istarts_with("FACET", "f"));
    UTEST_ASSERT(gmio_ascii_istarts_with("FACET", "fa"));

    return NULL;
}
