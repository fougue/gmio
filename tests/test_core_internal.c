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

#include "core_utils.h"
#include "stream_buffer.h"

#include "../src/gmio_core/internal/byte_codec.h"
#include "../src/gmio_core/internal/byte_swap.h"
#include "../src/gmio_core/internal/convert.h"
#include "../src/gmio_core/internal/fast_atof.h"
#include "../src/gmio_core/internal/safe_cast.h"
#include "../src/gmio_core/internal/string_parse.h"
#include "../src/gmio_core/internal/string_utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const char* test_internal__byte_swap()
{
    UTEST_ASSERT(gmio_uint16_bswap(0x1122) == 0x2211);
    UTEST_ASSERT(gmio_uint32_bswap(0x11223344) == 0x44332211);
    return NULL;
}

const char* test_internal__byte_codec()
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

static gmio_bool_t gmio_test_calculation_atof(const char* value_str)
{
    const gmio_float32_t fast_value = fast_atof(value_str, NULL);
    const gmio_float32_t std_value = (gmio_float32_t)strtod(value_str, NULL);
    const gmio_bool_t accurate =
            gmio_float32_equals_by_ulp(fast_value, std_value, 1);
    if (!accurate) {
        fprintf(stderr,
                "*** ERROR: fast_atof() less accurate than strtod()\n"
                "    value_str : \"%s\"\n"
                "    fast_value: %.12f (%s) as_int: 0x%x\n"
                "    std_value : %.12f (%s) as_int: 0x%x\n"
                "    ulp_diff  : %u\n",
                value_str,
                fast_value,
                gmio_float32_sign(fast_value) > 0 ? "+" : "-",
                gmio_convert_uint32(fast_value),
                std_value,
                gmio_float32_sign(std_value) > 0 ? "+" : "-",
                gmio_convert_uint32(std_value),
                gmio_float32_ulp_diff(fast_value, std_value));
    }
    return accurate;
}

const char* test_internal__fast_atof()
{
    gmio_bool_t accurate = GMIO_TRUE;

    accurate &= gmio_test_calculation_atof("340282346638528859811704183484516925440.000000");
    accurate &= gmio_test_calculation_atof("3.402823466e+38F");
    accurate &= gmio_test_calculation_atof("3402823466e+29F");
    accurate &= gmio_test_calculation_atof("-340282346638528859811704183484516925440.000000");
    accurate &= gmio_test_calculation_atof("-3.402823466e+38F");
    accurate &= gmio_test_calculation_atof("-3402823466e+29F");
    accurate &= gmio_test_calculation_atof("34028234663852885981170418348451692544.000000");
    accurate &= gmio_test_calculation_atof("3.402823466e+37F");
    accurate &= gmio_test_calculation_atof("3402823466e+28F");
    accurate &= gmio_test_calculation_atof("-34028234663852885981170418348451692544.000000");
    accurate &= gmio_test_calculation_atof("-3.402823466e+37F");
    accurate &= gmio_test_calculation_atof("-3402823466e+28F");
    accurate &= gmio_test_calculation_atof(".00234567");
    accurate &= gmio_test_calculation_atof("-.00234567");
    accurate &= gmio_test_calculation_atof("0.00234567");
    accurate &= gmio_test_calculation_atof("-0.00234567");
    accurate &= gmio_test_calculation_atof("1.175494351e-38F");
#if 0
    /* This check fails */
    accurate &= gmio_test_calculation_atof("1175494351e-47F");
#endif
    accurate &= gmio_test_calculation_atof("1.175494351e-37F");
    accurate &= gmio_test_calculation_atof("1.175494351e-36F");
    accurate &= gmio_test_calculation_atof("-1.175494351e-36F");
    accurate &= gmio_test_calculation_atof("123456.789");
    accurate &= gmio_test_calculation_atof("-123456.789");
    accurate &= gmio_test_calculation_atof("0000123456.789");
    accurate &= gmio_test_calculation_atof("-0000123456.789");
    accurate &= gmio_test_calculation_atof("-0.0690462109446526");

    UTEST_ASSERT(accurate == GMIO_TRUE);

    return NULL;
}

const char* test_internal__safe_cast()
{
#if GMIO_TARGET_ARCH_BIT_SIZE > 32
    const size_t maxUInt32 = 0xFFFFFFFF;
    UTEST_ASSERT(gmio_size_to_uint32(maxUInt32 + 1) == 0xFFFFFFFF);
    UTEST_ASSERT(gmio_size_to_uint32(0xFFFFFFFF) == 0xFFFFFFFF);
    UTEST_ASSERT(gmio_size_to_uint32(100) == 100);
#endif
    return NULL;
}

const char* test_internal__string_parse()
{
    const char text[] =
            "Une    citation,\to je crois qu'elle est de moi :"
            "Parfois le chemin est rude.\n"
            "pi : 3.1415926535897932384626433832795";

    {
        gmio_stream_buffer_t buff = {0};
        gmio_stream_t stream = {0};

        char small_fwd_it_str[4];
        char fwd_it_str[32];
        gmio_string_stream_fwd_iterator_t fwd_it = {0};

        char copy_str[128];
        gmio_string_t copy_strbuff;

        buff.readonly_ptr = text;
        buff.len = strlen(text);
        buff.pos = 0;
        gmio_stream_set_buffer(&stream, &buff);

        fwd_it.stream = &stream;
        fwd_it.buffer.ptr = fwd_it_str;
        fwd_it.buffer.max_len = sizeof(fwd_it_str);
        gmio_string_stream_fwd_iterator_init(&fwd_it);

        copy_strbuff.ptr = copy_str;
        copy_strbuff.max_len = sizeof(copy_str);

        UTEST_ASSERT(gmio_current_char(&fwd_it) != NULL);
        UTEST_ASSERT(*gmio_current_char(&fwd_it) == 'U');

        copy_strbuff.len = 0;
        UTEST_ASSERT(gmio_eat_word(&fwd_it, &copy_strbuff) == 0);
        /* printf("\ncopy_strbuff.ptr = \"%s\"\n", copy_strbuff.ptr); */
        UTEST_ASSERT(strcmp(copy_strbuff.ptr, "Une") == 0);

        copy_strbuff.len = 0;
        UTEST_ASSERT(gmio_eat_word(&fwd_it, &copy_strbuff) == 0);
        UTEST_ASSERT(strcmp(copy_strbuff.ptr, "citation,") == 0);

        copy_strbuff.len = 0;
        UTEST_ASSERT(gmio_eat_word(&fwd_it, &copy_strbuff) == 0);
        UTEST_ASSERT(strcmp(copy_strbuff.ptr, "o") == 0);

        copy_strbuff.len = 0;
        UTEST_ASSERT(gmio_eat_word(&fwd_it, &copy_strbuff) == 0);
        UTEST_ASSERT(strcmp(copy_strbuff.ptr, "je") == 0);

        gmio_skip_spaces(&fwd_it);
        UTEST_ASSERT(gmio_next_char(&fwd_it) != NULL);
        UTEST_ASSERT(*gmio_current_char(&fwd_it) == 'r');

        /* Test with very small string buffer */
        buff.pos = 0;
        fwd_it.buffer.ptr = small_fwd_it_str;
        fwd_it.buffer.max_len = sizeof(small_fwd_it_str);
        gmio_string_stream_fwd_iterator_init(&fwd_it);

        UTEST_ASSERT(*gmio_current_char(&fwd_it) == 'U');
        copy_strbuff.len = 0;
        UTEST_ASSERT(gmio_eat_word(&fwd_it, &copy_strbuff) == 0);
        copy_strbuff.len = 0;
        UTEST_ASSERT(gmio_eat_word(&fwd_it, &copy_strbuff) == 0);
        UTEST_ASSERT(strcmp(copy_strbuff.ptr, "citation,") == 0);
    }

    {
        gmio_stream_buffer_t buff = {0};
        gmio_stream_t stream = {0};

        char fwd_it_str[32];
        gmio_string_stream_fwd_iterator_t fwd_it = {0};

        char copy_str[128];
        gmio_string_t copy_strbuff;

        buff.readonly_ptr = text;
        buff.len = strlen(text);
        buff.pos = 0;
        gmio_stream_set_buffer(&stream, &buff);

        fwd_it.stream = &stream;
        fwd_it.buffer.ptr = fwd_it_str;
        fwd_it.buffer.max_len = sizeof(fwd_it_str);
        gmio_string_stream_fwd_iterator_init(&fwd_it);

        copy_strbuff.ptr = copy_str;
        copy_strbuff.len = 0;
        copy_strbuff.max_len = sizeof(copy_str);

        UTEST_ASSERT(gmio_eat_word(&fwd_it, &copy_strbuff) == 0);
        UTEST_ASSERT(strcmp(copy_strbuff.ptr, "Une") == 0);

        UTEST_ASSERT(gmio_eat_word(&fwd_it, &copy_strbuff) == 0);
        UTEST_ASSERT(strcmp(copy_strbuff.ptr, "Unecitation,") == 0);
        UTEST_ASSERT(gmio_eat_word(&fwd_it, &copy_strbuff) == 0);
        UTEST_ASSERT(strcmp(copy_strbuff.ptr, "Unecitation,o") == 0);
    }

    return NULL;
}

const char* test_internal__string_utils()
{
    char c; /* for loop counter */

    UTEST_ASSERT(gmio_clocale_isspace(' '));
    UTEST_ASSERT(gmio_clocale_isspace('\t'));
    UTEST_ASSERT(gmio_clocale_isspace('\n'));
    UTEST_ASSERT(gmio_clocale_isspace('\r'));

    for (c = 0; c >= 0 && c <= 127; ++c) {
        if (65 <= c && c <= 90) {
            UTEST_ASSERT(gmio_clocale_isupper(c));
        }
        else if (97 <= c && c <= 122) {
            UTEST_ASSERT(gmio_clocale_islower(c));
        }
        else if (c == 0x20 || (0x09 <= c && c <= 0x0d)) {
            UTEST_ASSERT(gmio_clocale_isspace(c));
        }
        else {
            UTEST_ASSERT(!gmio_clocale_isupper(c));
            UTEST_ASSERT(!gmio_clocale_islower(c));
            UTEST_ASSERT(!gmio_clocale_isspace(c));
        }
    }

    UTEST_ASSERT(gmio_clocale_tolower('A') == 'a');
    UTEST_ASSERT(gmio_clocale_tolower('Z') == 'z');
    UTEST_ASSERT(gmio_clocale_tolower('(') == '(');
    UTEST_ASSERT(gmio_clocale_toupper('a') == 'A');
    UTEST_ASSERT(gmio_clocale_toupper('z') == 'Z');
    UTEST_ASSERT(gmio_clocale_toupper('(') == '(');

    UTEST_ASSERT(gmio_clocale_char_iequals('a', 'a'));
    UTEST_ASSERT(gmio_clocale_char_iequals('a', 'A'));
    UTEST_ASSERT(gmio_clocale_char_iequals('A', 'a'));
    UTEST_ASSERT(gmio_clocale_char_iequals('{', '{'));
    UTEST_ASSERT(!gmio_clocale_char_iequals('{', '['));

    UTEST_ASSERT(gmio_stricmp("FACET", "facet") == 0);
    UTEST_ASSERT(gmio_stricmp("facet", "FACET") == 0);
    UTEST_ASSERT(gmio_stricmp("facet", "facet") == 0);
    UTEST_ASSERT(gmio_stricmp("FACET", "FACET") == 0);
    UTEST_ASSERT(gmio_stricmp("", "") == 0);
    UTEST_ASSERT(gmio_stricmp("", "facet") != 0);
    UTEST_ASSERT(gmio_stricmp("facet", "facet_") != 0);
    UTEST_ASSERT(gmio_stricmp("facet_", "facet") != 0);

    UTEST_ASSERT(gmio_istarts_with("facet", ""));
    UTEST_ASSERT(gmio_istarts_with("facet", "f"));
    UTEST_ASSERT(gmio_istarts_with("facet", "fa"));
    UTEST_ASSERT(gmio_istarts_with("facet", "facet"));
    UTEST_ASSERT(!gmio_istarts_with("facet", "a"));
    UTEST_ASSERT(!gmio_istarts_with("facet", " facet"));
    UTEST_ASSERT(gmio_istarts_with("facet", "F"));
    UTEST_ASSERT(gmio_istarts_with("FACET", "f"));
    UTEST_ASSERT(gmio_istarts_with("FACET", "fa"));

    return NULL;
}
