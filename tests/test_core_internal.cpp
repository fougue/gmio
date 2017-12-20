/****************************************************************************
** Copyright (c) 2017, Fougue Ltd. <http://www.fougue.pro>
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
**     1. Redistributions of source code must retain the above copyright
**        notice, this list of conditions and the following disclaimer.
**
**     2. Redistributions in binary form must reproduce the above
**        copyright notice, this list of conditions and the following
**        disclaimer in the documentation and/or other materials provided
**        with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
****************************************************************************/

#include "utest_assert.h"

#include "../src/gmio_core/error.h"
#include "../src/gmio_core/internal/byte_codec.h"
#include "../src/gmio_core/internal/byte_swap.h"
#include "../src/gmio_core/internal/convert.h"
#include "../src/gmio_core/internal/fast_atof.h"
#include "../src/gmio_core/internal/file_utils.h"
#include "../src/gmio_core/internal/itoa.h"
#include "../src/gmio_core/internal/locale_utils.h"
#include "../src/gmio_core/internal/numeric_utils.h"
#include "../src/gmio_core/internal/safe_cast.h"
#include "../src/gmio_core/internal/string_ascii_utils.h"
#include "../src/gmio_core/internal/text_istream.h"
#include "../src/gmio_core/internal/zip_utils.h"
#include "../src/gmio_core/internal/zlib_utils.h"

#include <clocale>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <limits>
#include <sstream>
#include <utility>

namespace gmio {

namespace {

void TestCoreInternal_fprintf_atof_err(
        const char* func_fast_atof_str,
        const char* val_str,
        float fast_val,
        float std_val)
{
    std::fprintf(stderr,
            "*** ERROR: %s() less accurate than strtod()\n"
            "    value_str : \"%s\"\n"
            "    fast_value: %.12f (%s) as_int: 0x%x\n"
            "    std_value : %.12f (%s) as_int: 0x%x\n"
            "    ulp_diff  : %u\n",
            func_fast_atof_str,
            val_str,
            fast_val,
            float32_sign(fast_val) > 0 ? "+" : "-",
            convert_uint32(fast_val),
            std_val,
            float32_sign(std_val) > 0 ? "+" : "-",
            convert_uint32(std_val),
            float32_ulpDiff(fast_val, std_val));
}

bool TestCoreInternal_check_atof(const char* val_str)
{
    const float std_val = std::strtof(val_str, nullptr);
    int accurate_count = 0;

    { // Test fast_atof()
        const float fast_val = fast_atof(val_str);
        if (float32_ulpEquals(fast_val, std_val, 1))
            ++accurate_count;
        else
            TestCoreInternal_fprintf_atof_err("fast_atof", val_str, fast_val, std_val);
    }

    { // Test gmio_stringstream_fast_atof()
#if 0
        char iobuff[512] = {};
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
            TestCoreInternal_fprintf_atof_err(
                        "gmio_stringstream_fast_atof", val_str, fast_val, std_val);
        }
#else
        ++accurate_count;
#endif
    }

    return accurate_count == 2;
}

const char* __tc__test_internal__locale_utils()
{
    const char* lc = std::setlocale(LC_NUMERIC, "");
    if (lc != nullptr
            && ascii_stricmp(lc, "C") != 0
            && ascii_stricmp(lc, "POSIX") != 0)
    {
        UTEST_ASSERT(!lc_numeric_is_C());
    }
    else {
        std::cerr << "\nskip: default locale is NULL or already C/POSIX";
    }

    lc = std::setlocale(LC_NUMERIC, "C");
    if (lc != nullptr) {
        UTEST_ASSERT(lc_numeric_is_C());
    }

    return nullptr;
}

} // namespace

static const char* TestCoreInternal_byteSwap()
{
    UTEST_ASSERT(uint16_byteSwap(0x1122) == 0x2211);
    UTEST_ASSERT(uint32_byteSwap(0x11223344) == 0x44332211);
    return nullptr;
}

static const char* TestCoreInternal_byteCodec()
{
    { // decode
        const uint8_t data[] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };
        UTEST_ASSERT(uint16_decodeLittleEndian(data) == 0x2211);
        UTEST_ASSERT(uint32_decodeLittleEndian(data) == 0x44332211);
        UTEST_ASSERT(uint64_decodeLittleEndian(data) == 0x8877665544332211);
        UTEST_ASSERT(uint16_decodeBigEndian(data) == 0x1122);
        UTEST_ASSERT(uint32_decodeBigEndian(data) == 0x11223344);
        UTEST_ASSERT(uint64_decodeBigEndian(data) == 0x1122334455667788);
    }

    { // encode
        uint8_t bytes[8] = {};

        uint16_encodeLittleEndian(0x1122, bytes);
        UTEST_ASSERT(bytes[0] == 0x22 && bytes[1] == 0x11);
        uint16_encodeBigEndian(0x1122, bytes);
        UTEST_ASSERT(bytes[0] == 0x11 && bytes[1] == 0x22);

        uint32_encodeLittleEndian(0x11223344, bytes);
        static const uint8_t bytes_uint32_le[] = { 0x44, 0x33, 0x22, 0x11 };
        UTEST_ASSERT(std::memcmp(bytes, bytes_uint32_le, 4) == 0);

        uint32_encodeBigEndian(0x11223344, bytes);
        static const uint8_t bytes_uint32_be[] = { 0x11, 0x22, 0x33, 0x44 };
        UTEST_ASSERT(std::memcmp(bytes, bytes_uint32_be, 4) == 0);

        uint64_encodeLittleEndian(0x1122334455667788, bytes);
        static const uint8_t bytes_uint64_le[] =
                { 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11 };
        UTEST_ASSERT(std::memcmp(bytes, bytes_uint64_le, 8) == 0);

        uint64_encodeBigEndian(0x1122334455667788, bytes);
        static const uint8_t bytes_uint64_be[] =
                { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };
        UTEST_ASSERT(std::memcmp(bytes, bytes_uint64_be, 8) == 0);
    }

    return nullptr;
}

static const char* TestCoreInternal_fast_atof()
{
    bool ok = true;

    ok = ok && TestCoreInternal_check_atof("340282346638528859811704183484516925440.000000");
    ok = ok && TestCoreInternal_check_atof("3.402823466e+38F");
    ok = ok && TestCoreInternal_check_atof("3402823466e+29F");
    ok = ok && TestCoreInternal_check_atof("-340282346638528859811704183484516925440.000000");
    ok = ok && TestCoreInternal_check_atof("-3.402823466e+38F");
    ok = ok && TestCoreInternal_check_atof("-3402823466e+29F");
    ok = ok && TestCoreInternal_check_atof("34028234663852885981170418348451692544.000000");
    ok = ok && TestCoreInternal_check_atof("3.402823466e+37F");
    ok = ok && TestCoreInternal_check_atof("3402823466e+28F");
    ok = ok && TestCoreInternal_check_atof("-34028234663852885981170418348451692544.000000");
    ok = ok && TestCoreInternal_check_atof("-3.402823466e+37F");
    ok = ok && TestCoreInternal_check_atof("-3402823466e+28F");
    ok = ok && TestCoreInternal_check_atof(".00234567");
    ok = ok && TestCoreInternal_check_atof("-.00234567");
    ok = ok && TestCoreInternal_check_atof("0.00234567");
    ok = ok && TestCoreInternal_check_atof("-0.00234567");
    ok = ok && TestCoreInternal_check_atof("1.175494351e-38F");
#if 0
    // This check fails
    ok = ok && TestCoreInternal_check_atof("1175494351e-47F");
#endif
    ok = ok && TestCoreInternal_check_atof("1.175494351e-37F");
    ok = ok && TestCoreInternal_check_atof("1.175494351e-36F");
    ok = ok && TestCoreInternal_check_atof("-1.175494351e-36F");
    ok = ok && TestCoreInternal_check_atof("123456.789");
    ok = ok && TestCoreInternal_check_atof("-123456.789");
    ok = ok && TestCoreInternal_check_atof("0000123456.789");
    ok = ok && TestCoreInternal_check_atof("-0000123456.789");
    ok = ok && TestCoreInternal_check_atof("-0.0690462109446526");

    UTEST_ASSERT(ok);

    return nullptr;
}

static const char* TestCoreInternal_safeCast()
{
#if GMIO_TARGET_ARCH_BIT_SIZE > 32
    const size_t maxUInt32 = 0xFFFFFFFF;
    UTEST_ASSERT(size_to_uint32(maxUInt32 + 1) == 0xFFFFFFFF);
    UTEST_ASSERT(size_to_uint32(0xFFFFFFFF) == 0xFFFFFFFF);
    UTEST_ASSERT(size_to_uint32(100) == 100);
#endif

    return nullptr;
}

static const char* TestCoreInternal_TextIStream()
{
    static const std::string text =
            "Une    citation,\to je crois qu'elle est de moi :"
            "Parfois le chemin est rude.\n"
            "pi : 3.1415926535897932384626433832795";
    std::istringstream istrstream(text);
    {
        char cstr[32];
        TextIStream sstream(makeSpan(cstr), istream_funcReadData(&istrstream));

        UTEST_ASSERT(sstream.currentChar() != nullptr);
        UTEST_ASSERT(*sstream.currentChar() == 'U');

        UTEST_ASSERT(sstream.eatWord() == "Une");
        UTEST_ASSERT(sstream.eatWord() == "citation,");
        UTEST_ASSERT(sstream.eatWord() == "o");
        UTEST_ASSERT(sstream.eatWord() == "je");

        sstream.skipAsciiSpaces();
        UTEST_ASSERT(sstream.nextChar() != nullptr);
        UTEST_ASSERT(*sstream.currentChar() == 'r');
    }

    istrstream.seekg(0);
    {   // Test with very small buffer
        char cstr[4];
        TextIStream sstream(makeSpan(cstr), istream_funcReadData(&istrstream));

        UTEST_ASSERT(*sstream.currentChar() == 'U');
        sstream.eatWord();
        UTEST_ASSERT(sstream.eatWord() == "citation,");
    }

    return nullptr;
}

static const char* TestCoreInternal_ostringstream()
{
#if 0
    static const size_t size = 8192;
    char* input = g_testcore_memblock.ptr;
    char* output = (char*)g_testcore_memblock.ptr + size;
    char strbuff[256] = {0};
    struct gmio_rw_buffer rwbuff = gmio_rw_buffer(output, size, 0);
    struct gmio_ostringstream sstream =
            gmio_ostringstream(
                gmio_stream_buffer(&rwbuff),
                gmio_string(strbuff, 0, sizeof(strbuff) - 1));

    {   /* Create "input" string */
        size_t i = 0;
        for (i = 0; i < size; ++i) {
            const char c = 32 + (i % 94); /* Printable ASCII chars */
            input[i] = c;
        }
        /* Test gmio_ostringstream_write_char() */
        for (i = 0; i < size; ++i)
            gmio_ostringstream_write_char(&sstream, input[i]);
        gmio_ostringstream_flush(&sstream);
        UTEST_ASSERT(std::strncmp(input, output, size) == 0);

        /* Test gmio_ostringstream_write_[ui]32() */
        {
            static const char result[] =
                    "20 12345 0 -1 -12345678 4294967295 2147483647";
            static const unsigned result_len = sizeof(result) - 1;
            rwbuff.pos = 0;
            gmio_ostringstream_write_u32(&sstream, 20);
            gmio_ostringstream_write_char(&sstream, ' ');
            gmio_ostringstream_write_u32(&sstream, 12345);
            gmio_ostringstream_write_char(&sstream, ' ');
            gmio_ostringstream_write_u32(&sstream, 0);
            gmio_ostringstream_write_char(&sstream, ' ');
            gmio_ostringstream_write_i32(&sstream, -1);
            gmio_ostringstream_write_char(&sstream, ' ');
            gmio_ostringstream_write_i32(&sstream, -12345678);
            gmio_ostringstream_write_char(&sstream, ' ');
            gmio_ostringstream_write_u32(&sstream, (uint32_t)-1);
            gmio_ostringstream_write_char(&sstream, ' ');
            gmio_ostringstream_write_i32(&sstream, ((uint32_t)1 << 31) - 1);
            gmio_ostringstream_flush(&sstream);
            UTEST_ASSERT(std::strncmp(result, sstream.strbuff.ptr, result_len) == 0);
            UTEST_ASSERT(std::strncmp(result, output, result_len) == 0);
        }

        /* Test gmio_ostringstream_write_base64() */
        {
            static const char str[] = "Fougue+gmio";
            static const char str_b64[] = "Rm91Z3VlK2dtaW8=";
            static const unsigned str_len = sizeof(str) - 1;
            static const unsigned str_b64_len = sizeof(str_b64) - 1;
            rwbuff.pos = 0;
            gmio_ostringstream_write_base64(
                        &sstream, (unsigned const char*)str, str_len);
            gmio_ostringstream_flush(&sstream);
            UTEST_ASSERT(std::strncmp(str_b64, sstream.strbuff.ptr, str_b64_len) == 0);
            UTEST_ASSERT(std::strncmp(str_b64, output, str_b64_len) == 0);
        }

        /* Test gmio_ostringstream_write_xml...() */
        {
            static const char result[] =
                    " foo=\"crac\" bar=\"456789\"<![CDATA[]]>";
            static const unsigned result_len = sizeof(result) - 1;
            rwbuff.pos = 0;
            gmio_ostringstream_write_xmlattr_str(&sstream, "foo", "crac");
            gmio_ostringstream_write_xmlattr_u32(&sstream, "bar", 456789);
            gmio_ostringstream_write_xmlcdata_open(&sstream);
            gmio_ostringstream_write_xmlcdata_close(&sstream);
            gmio_ostringstream_flush(&sstream);
            UTEST_ASSERT(std::strncmp(result, sstream.strbuff.ptr, result_len) == 0);
            UTEST_ASSERT(std::strncmp(result, output, result_len) == 0);
        }
    }
#endif

    return NULL;
}

static const char* TestCoreInternal_string_ascii_utils()
{
    UTEST_ASSERT(ascii_isSpace(' '));
    UTEST_ASSERT(ascii_isSpace('\t'));
    UTEST_ASSERT(ascii_isSpace('\n'));
    UTEST_ASSERT(ascii_isSpace('\r'));

    for (char c = 0; 0 <= c && c <= 127; ++c) {
        if (65 <= c && c <= 90) {
            UTEST_ASSERT(ascii_isUpper(c));
        }
        else if (97 <= c && c <= 122) {
            UTEST_ASSERT(ascii_isLower(c));
        }
        else if (c == 0x20 || (0x09 <= c && c <= 0x0d)) {
            UTEST_ASSERT(ascii_isSpace(c));
        }
        else if (48 <= c && c <= 57) {
            UTEST_ASSERT(ascii_isDigit(c));
        }
        else {
            UTEST_ASSERT(!ascii_isUpper(c));
            UTEST_ASSERT(!ascii_isLower(c));
            UTEST_ASSERT(!ascii_isSpace(c));
            UTEST_ASSERT(!ascii_isDigit(c));
        }
    }

    UTEST_ASSERT(ascii_toLower('A') == 'a');
    UTEST_ASSERT(ascii_toLower('Z') == 'z');
    UTEST_ASSERT(ascii_toLower('(') == '(');
    UTEST_ASSERT(ascii_toUpper('a') == 'A');
    UTEST_ASSERT(ascii_toUpper('z') == 'Z');
    UTEST_ASSERT(ascii_toUpper('(') == '(');

    UTEST_ASSERT(ascii_iequals('a', 'a'));
    UTEST_ASSERT(ascii_iequals('a', 'A'));
    UTEST_ASSERT(ascii_iequals('A', 'a'));
    UTEST_ASSERT(ascii_iequals('{', '{'));
    UTEST_ASSERT(!ascii_iequals('{', '['));

    UTEST_ASSERT(ascii_stricmp("FACET", "facet") == 0);
    UTEST_ASSERT(ascii_stricmp("facet", "FACET") == 0);
    UTEST_ASSERT(ascii_stricmp("facet", "facet") == 0);
    UTEST_ASSERT(ascii_stricmp("FACET", "FACET") == 0);
    UTEST_ASSERT(ascii_stricmp("", "") == 0);
    UTEST_ASSERT(ascii_stricmp("", "facet") != 0);
    UTEST_ASSERT(ascii_stricmp("facet", "facet_") != 0);
    UTEST_ASSERT(ascii_stricmp("facet_", "facet") != 0);

    UTEST_ASSERT(ascii_istartsWith("facet", ""));
    UTEST_ASSERT(ascii_istartsWith("facet", "f"));
    UTEST_ASSERT(ascii_istartsWith("facet", "fa"));
    UTEST_ASSERT(ascii_istartsWith("facet", "facet"));
    UTEST_ASSERT(!ascii_istartsWith("facet", "a"));
    UTEST_ASSERT(!ascii_istartsWith("facet", " facet"));
    UTEST_ASSERT(ascii_istartsWith("facet", "F"));
    UTEST_ASSERT(ascii_istartsWith("FACET", "f"));
    UTEST_ASSERT(ascii_istartsWith("FACET", "fa"));

    {
        const char str[] = "solid test, FACET contents";
        const char* substr = ascii_istrstr(str, "facet");
        //std::cout << "\nsubstr: " << substr << '\n';
        UTEST_ASSERT(std::strncmp(substr, "FACET", 5) == 0);
    }

    {
        static const std::pair<const char*, const char*> array_str_case[] = {
            { "test this string", "test this string" },
            { "", "" },
            { " ", "" },
            { " \t \n ", "" },
            { "a ", "a" },
            { "a b    c d    \t  ", "a b    c d" }
        };
        for (const auto& str_case : array_str_case) {
            {
                std::string str = str_case.first;
                ascii_trimEnd(&str);
                UTEST_COMPARE_CSTR(str_case.second, str.data());
            }
            {
                char str[512] = {};
                std::strncpy(str, str_case.first, sizeof(str));
                ascii_trimEnd(str, std::strlen(str_case.first));
                UTEST_COMPARE_CSTR(str_case.second, str);
            }
        }
    }

    return nullptr;
}

static const char* TestCoreInternal_locale_utils()
{
    const char* error_str = nullptr;
    lc_numeric_save();
    std::printf("\ninfo: current locale is \"%s\"", lc_numeric());
    error_str = __tc__test_internal__locale_utils();
    lc_numeric_restore();
    return error_str;
}

#if 0
static void __tc__write_file(
        const char* filepath, const uint8_t* bytes, size_t len)
{
    FILE* file = std::fopen(filepath, "wb");
    std::fwrite(bytes, 1, len, file);
    std::fclose(file);
}

struct __tc__func_write_file_data_cookie {
    struct gmio_stream* stream;
    const uint8_t* data;
    const uint8_t* zdata;
    uint32_t data_crc32;
    size_t data_len;
    size_t zdata_len;
};

static int __tc__write_zip_file_data(
        void* cookie, struct gmio_zip_data_descriptor* dd)
{
    struct __tc__func_write_file_data_cookie* fcookie =
            (struct __tc__func_write_file_data_cookie*)cookie;
    gmio_stream_write_bytes(fcookie->stream, fcookie->zdata, fcookie->zdata_len);
    dd->crc32 = fcookie->data_crc32;
    dd->uncompressed_size = fcookie->data_len;
    dd->compressed_size = fcookie->zdata_len;
    return GMIO_ERROR_OK;
}

static const char* __tc__zip_compare_entry(
        const struct gmio_zip_file_entry* lhs,
        const struct gmio_zip_file_entry* rhs)
{
    UTEST_COMPARE(lhs->compress_method, rhs->compress_method);
    UTEST_COMPARE(lhs->feature_version, rhs->feature_version);
    UTEST_COMPARE(lhs->filename_len, rhs->filename_len);
    UTEST_ASSERT(std::strncmp(lhs->filename, rhs->filename, lhs->filename_len) == 0);
    return NULL;
}
#endif

static const char* TestCoreInternal_zip_utils()
{
#if 0
    static const unsigned bytes_size = 1024;
    uint8_t* bytes = g_testcore_memblock.ptr;
    struct gmio_rw_buffer wbuff = gmio_rw_buffer(bytes, bytes_size, 0);
    struct gmio_stream stream = gmio_stream_buffer(&wbuff);
    int error;

    /* Write empty ZIP file */
    {
        struct gmio_zip_end_of_central_directory_record eocdr = {0};
        gmio_zip_write_end_of_central_directory_record(&stream, &eocdr, &error);
        UTEST_COMPARE(error, GMIO_ERROR_OK);
#if 1
        __tc__write_file("test_output_empty.zip", wbuff.ptr, wbuff.pos);
#endif
    }

    /* Write empty Zip64 file */
    wbuff.pos = 0;
    {
        struct gmio_zip64_end_of_central_directory_record eocdr64 = {0};
        eocdr64.version_needed_to_extract =
                GMIO_ZIP_FEATURE_VERSION_FILE_ZIP64_FORMAT_EXTENSIONS;
        gmio_zip64_write_end_of_central_directory_record(
            &stream, &eocdr64, &error);
        UTEST_COMPARE(error, GMIO_ERROR_OK);

        struct gmio_zip64_end_of_central_directory_locator eocdl64 = {0};
        gmio_zip64_write_end_of_central_directory_locator(
                    &stream, &eocdl64, &error);
        UTEST_COMPARE(error, GMIO_ERROR_OK);

        struct gmio_zip_end_of_central_directory_record eocdr = {0};
        gmio_zip_write_end_of_central_directory_record(&stream, &eocdr, &error);
        UTEST_COMPARE(error, GMIO_ERROR_OK);
#if 1
        __tc__write_file("test_output_empty_64.zip", wbuff.ptr, wbuff.pos);
#endif
    }

    /* Common constants */
    static const char zip_entry_filedata[] =
            "On ne fait bien que ce qu'on fait soi-même";
    struct __tc__func_write_file_data_cookie fcookie = {0};
    fcookie.stream = &stream;
    fcookie.data = (const uint8_t*)zip_entry_filedata;
    fcookie.zdata = fcookie.data; /* No compression */
    fcookie.data_len = GMIO_ARRAY_SIZE(zip_entry_filedata) - 1;
    fcookie.zdata_len = fcookie.data_len; /* No compression */
    fcookie.data_crc32 = gmio_zlib_crc32(fcookie.data, fcookie.data_len);
    static const char zip_entry_filename[] = "file.txt";
    static const uint16_t zip_entry_filename_len =
            GMIO_ARRAY_SIZE(zip_entry_filename) - 1;

    /* Common variables */
    struct gmio_zip_file_entry entry = {0};
    entry.compress_method = GMIO_ZIP_COMPRESS_METHOD_NO_COMPRESSION;
    entry.feature_version = GMIO_ZIP_FEATURE_VERSION_DEFAULT;
    entry.filename = zip_entry_filename;
    entry.filename_len = zip_entry_filename_len;
    entry.cookie_func_write_file_data = &fcookie;
    entry.func_write_file_data = __tc__write_zip_file_data;

    /*
     * Write one-entry ZIP file
     */
    {
        wbuff.pos = 0;
        entry.feature_version = GMIO_ZIP_FEATURE_VERSION_DEFAULT;
        UTEST_ASSERT(gmio_zip_write_single_file(&stream, &entry, &error));
        UTEST_COMPARE(error, GMIO_ERROR_OK);
#if 1
        __tc__write_file("test_output_one_file.zip", wbuff.ptr, wbuff.pos);
#endif

        const uintmax_t zip_archive_len = wbuff.pos;
        wbuff.pos = 0;
        /* -- Read ZIP local file header */
        struct gmio_zip_local_file_header zip_lfh = {0};
        const size_t lfh_read_len =
                gmio_zip_read_local_file_header(&stream, &zip_lfh, &error);
        UTEST_COMPARE(error, GMIO_ERROR_OK);
        struct gmio_zip_file_entry lfh_entry = {0};
        lfh_entry.compress_method = zip_lfh.compress_method;
        lfh_entry.feature_version = zip_lfh.version_needed_to_extract;
        lfh_entry.filename = (const char*)wbuff.ptr + wbuff.pos;
        lfh_entry.filename_len = zip_lfh.filename_len;
        const char* check_str = __tc__zip_compare_entry(&entry, &lfh_entry);
        if (check_str != NULL) return check_str;
        /* -- Read ZIP end of central directory record */
        wbuff.pos =
                zip_archive_len - GMIO_ZIP_SIZE_END_OF_CENTRAL_DIRECTORY_RECORD;
        struct gmio_zip_end_of_central_directory_record zip_eocdr = {0};
        gmio_zip_read_end_of_central_directory_record(
                    &stream, &zip_eocdr, &error);
        UTEST_COMPARE(error, GMIO_ERROR_OK);
        UTEST_COMPARE(zip_eocdr.entry_count, 1);
        /* -- Read ZIP central directory */
        wbuff.pos = zip_eocdr.central_dir_offset;
        struct gmio_zip_central_directory_header zip_cdh = {0};
        gmio_zip_read_central_directory_header(&stream, &zip_cdh, &error);
        struct gmio_zip_file_entry cdh_entry = {0};
        cdh_entry.compress_method = zip_cdh.compress_method;
        cdh_entry.feature_version = zip_cdh.version_needed_to_extract;
        cdh_entry.filename = (const char*)wbuff.ptr + wbuff.pos;
        cdh_entry.filename_len = zip_lfh.filename_len;
        UTEST_COMPARE(error, GMIO_ERROR_OK);
        UTEST_ASSERT((zip_cdh.general_purpose_flags &
                      GMIO_ZIP_GENERAL_PURPOSE_FLAG_USE_DATA_DESCRIPTOR)
                     != 0);
        UTEST_COMPARE(fcookie.data_crc32, zip_cdh.crc32);
        UTEST_COMPARE(fcookie.data_len, zip_cdh.uncompressed_size);
        UTEST_COMPARE(fcookie.zdata_len, zip_cdh.compressed_size);
        UTEST_COMPARE(zip_cdh.local_header_offset, 0);
        check_str = __tc__zip_compare_entry(&entry, &cdh_entry);
        if (check_str != NULL) return check_str;
        /* -- Read file data */
        const size_t pos_file_data =
                lfh_read_len + zip_lfh.filename_len + zip_lfh.extrafield_len;
        UTEST_ASSERT(std::strncmp((const char*)wbuff.ptr + pos_file_data,
                             (const char*)fcookie.zdata,
                             fcookie.zdata_len)
                     == 0);
        /* -- Read ZIP data descriptor */
        wbuff.pos = pos_file_data + zip_cdh.compressed_size;
        struct gmio_zip_data_descriptor zip_dd = {0};
        gmio_zip_read_data_descriptor(&stream, &zip_dd, &error);
        UTEST_COMPARE(error, GMIO_ERROR_OK);
        UTEST_COMPARE(zip_dd.crc32, zip_cdh.crc32);
        UTEST_COMPARE(zip_dd.compressed_size, zip_cdh.compressed_size);
        UTEST_COMPARE(zip_dd.uncompressed_size, zip_cdh.uncompressed_size);
    }

    /*
     * Write one-entry Zip64 file
     */
    {
        wbuff.pos = 0;
        entry.feature_version =
                GMIO_ZIP_FEATURE_VERSION_FILE_ZIP64_FORMAT_EXTENSIONS;
        UTEST_ASSERT(gmio_zip_write_single_file(&stream, &entry, &error));
        UTEST_COMPARE(error, GMIO_ERROR_OK);
#if 1
        __tc__write_file("test_output_one_file_64.zip", wbuff.ptr, wbuff.pos);
#endif

        const uintmax_t zip_archive_len = wbuff.pos;
        /* -- Read ZIP end of central directory record */
        wbuff.pos =
                zip_archive_len - GMIO_ZIP_SIZE_END_OF_CENTRAL_DIRECTORY_RECORD;
        struct gmio_zip_end_of_central_directory_record zip_eocdr = {0};
        gmio_zip_read_end_of_central_directory_record(
                    &stream, &zip_eocdr, &error);
        UTEST_COMPARE(GMIO_ERROR_OK, error);
        UTEST_COMPARE(UINT16_MAX, zip_eocdr.entry_count);
        UTEST_COMPARE(UINT32_MAX, zip_eocdr.central_dir_size);
        UTEST_COMPARE(UINT32_MAX, zip_eocdr.central_dir_offset);
        /* -- Read Zip64 end of central directory locator */
        wbuff.pos =
                zip_archive_len
                - GMIO_ZIP_SIZE_END_OF_CENTRAL_DIRECTORY_RECORD
                - GMIO_ZIP64_SIZE_END_OF_CENTRAL_DIRECTORY_LOCATOR;
        struct gmio_zip64_end_of_central_directory_locator zip64_eocdl = {0};
        gmio_zip64_read_end_of_central_directory_locator(
                    &stream, &zip64_eocdl, &error);
        UTEST_COMPARE(GMIO_ERROR_OK, error);
        /* -- Read Zip64 end of central directory record */
        wbuff.pos = zip64_eocdl.zip64_end_of_central_dir_offset;
        struct gmio_zip64_end_of_central_directory_record zip64_eocdr = {0};
        gmio_zip64_read_end_of_central_directory_record(
                    &stream, &zip64_eocdr, &error);
        UTEST_COMPARE(GMIO_ERROR_OK, error);
        UTEST_COMPARE(
                    GMIO_ZIP_FEATURE_VERSION_FILE_ZIP64_FORMAT_EXTENSIONS,
                    zip64_eocdr.version_needed_to_extract);
        UTEST_COMPARE(1, zip64_eocdr.entry_count);
        /* -- Read ZIP central directory header */
        wbuff.pos = zip64_eocdr.central_dir_offset;
        struct gmio_zip_central_directory_header zip_cdh = {0};
        size_t zip_cdh_len =
                gmio_zip_read_central_directory_header(&stream, &zip_cdh, &error);
        zip_cdh_len +=
                zip_cdh.filename_len
                + zip_cdh.extrafield_len
                + zip_cdh.filecomment_len;
        UTEST_COMPARE(GMIO_ERROR_OK, error);
        UTEST_COMPARE(zip64_eocdr.central_dir_size, zip_cdh_len);
        UTEST_ASSERT((zip_cdh.general_purpose_flags &
                      GMIO_ZIP_GENERAL_PURPOSE_FLAG_USE_DATA_DESCRIPTOR)
                     != 0);
        UTEST_COMPARE(fcookie.data_crc32, zip_cdh.crc32);
        UTEST_COMPARE(UINT32_MAX, zip_cdh.uncompressed_size);
        UTEST_COMPARE(UINT32_MAX, zip_cdh.compressed_size);
        UTEST_COMPARE(UINT32_MAX, zip_cdh.local_header_offset);
        struct gmio_zip_file_entry cdh_entry = {0};
        cdh_entry.compress_method = zip_cdh.compress_method;
        cdh_entry.feature_version = zip_cdh.version_needed_to_extract;
        cdh_entry.filename = (const char*)wbuff.ptr + wbuff.pos;
        cdh_entry.filename_len = zip_cdh.filename_len;
        const char* check_str = __tc__zip_compare_entry(&entry, &cdh_entry);
        if (check_str != NULL) return check_str;
        /* Read Zip64 extra field */
        wbuff.pos =
                zip64_eocdr.central_dir_offset
                + GMIO_ZIP_SIZE_CENTRAL_DIRECTORY_HEADER
                + zip_cdh.filename_len;
        struct gmio_zip64_extrafield zip64_extra = {0};
        gmio_zip64_read_extrafield(&stream, &zip64_extra, &error);
        UTEST_COMPARE(GMIO_ERROR_OK, error);
        UTEST_COMPARE(fcookie.data_len, zip64_extra.uncompressed_size);
        UTEST_COMPARE(fcookie.zdata_len, zip64_extra.compressed_size);
        UTEST_COMPARE(0, zip64_extra.local_header_offset);
        /* Read ZIP local file header */
        wbuff.pos = 0;
        struct gmio_zip_local_file_header zip_lfh = {0};
        gmio_zip_read_local_file_header(&stream, &zip_lfh, &error);
        UTEST_COMPARE(GMIO_ERROR_OK, error);
        UTEST_COMPARE(0, zip_lfh.crc32);
        UTEST_COMPARE(0, zip_lfh.compressed_size);
        UTEST_COMPARE(0, zip_lfh.uncompressed_size);
        /* Read ZIP local file header extrafield */
        wbuff.pos = GMIO_ZIP_SIZE_LOCAL_FILE_HEADER + zip_lfh.filename_len;
        gmio_zip64_read_extrafield(&stream, &zip64_extra, &error);
        UTEST_COMPARE(GMIO_ERROR_OK, error);
        UTEST_COMPARE(0, zip64_extra.uncompressed_size);
        UTEST_COMPARE(0, zip64_extra.compressed_size);
        UTEST_COMPARE(0, zip64_extra.local_header_offset);
        /* Read Zip64 data descriptor */
        wbuff.pos += fcookie.zdata_len;
        struct gmio_zip_data_descriptor zip_dd = {0};
        gmio_zip64_read_data_descriptor(&stream, &zip_dd, &error);
        UTEST_COMPARE(GMIO_ERROR_OK, error);
        UTEST_COMPARE(fcookie.data_crc32, zip_dd.crc32);
        UTEST_COMPARE(fcookie.data_len, zip_dd.uncompressed_size);
        UTEST_COMPARE(fcookie.zdata_len, zip_dd.compressed_size);
    }
#endif
    return nullptr;
}

static const char* TestCoreInternal_zlib_enumvalues()
{
    static const std::pair<int, ZLIB_CompressLevel> enumLevel[] = {
        { Z_BEST_SPEED, ZLIB_CompressLevel::BestSpeed },
        { Z_BEST_COMPRESSION, ZLIB_CompressLevel::BestSize },
        { 0, ZLIB_CompressLevel::Default },
        { -1, ZLIB_CompressLevel::None }
    };
    static const std::pair<int, ZLIB_CompressStrategy> enumStrategy[] = {
        { Z_DEFAULT_STRATEGY, ZLIB_CompressStrategy::Default },
        { Z_FILTERED, ZLIB_CompressStrategy::Filtered },
        { Z_HUFFMAN_ONLY, ZLIB_CompressStrategy::HuffmanOnly },
        { Z_RLE, ZLIB_CompressStrategy::RLE },
        { Z_FIXED, ZLIB_CompressStrategy::Fixed }
    };
    for (const auto& pair : enumLevel)
        UTEST_COMPARE(pair.first, static_cast<int>(pair.second));
    for (const auto& pair : enumStrategy)
        UTEST_COMPARE(pair.first, static_cast<int>(pair.second));
    return nullptr;
}

static const char* TestCoreInternal_file_utils()
{
#if 0
    struct gmio_const_string cstr = {0};

    cstr = gmio_fileutils_find_basefilename("");
    UTEST_ASSERT(gmio_const_string_is_empty(&cstr));

    cstr = gmio_fileutils_find_basefilename("file");
    UTEST_ASSERT(std::strncmp("file", cstr.ptr, cstr.len) == 0);

    cstr = gmio_fileutils_find_basefilename("file.txt");
    UTEST_ASSERT(std::strncmp("file", cstr.ptr, cstr.len) == 0);

    cstr = gmio_fileutils_find_basefilename("file.txt.zip");
    UTEST_ASSERT(std::strncmp("file.txt", cstr.ptr, cstr.len) == 0);

    cstr = gmio_fileutils_find_basefilename("/home/me/file.txt");
    UTEST_ASSERT(std::strncmp("file", cstr.ptr, cstr.len) == 0);

    cstr = gmio_fileutils_find_basefilename("/home/me/file.");
    UTEST_ASSERT(std::strncmp("file", cstr.ptr, cstr.len) == 0);

    cstr = gmio_fileutils_find_basefilename("/home/me/file");
    UTEST_ASSERT(std::strncmp("file", cstr.ptr, cstr.len) == 0);

    cstr = gmio_fileutils_find_basefilename("/home/me/file for you.txt");
    UTEST_ASSERT(std::strncmp("file for you", cstr.ptr, cstr.len) == 0);

    cstr = gmio_fileutils_find_basefilename("C:\\Program Files\\gmio\\file.txt");
    UTEST_ASSERT(std::strncmp("file", cstr.ptr, cstr.len) == 0);
#endif
    return nullptr;
}

static const char* TestCoreInternal_itoa()
{
    char buff[512] = {};
    u32toa(0, buff);
    UTEST_COMPARE_CSTR("0", buff);
    u32toa(100, buff);
    UTEST_COMPARE_CSTR("100", buff);
    u32toa(std::numeric_limits<uint32_t>::max(), buff);
    UTEST_COMPARE_CSTR("4294967295", buff);

    std::memset(buff, 0, sizeof(buff));
    i32toa(0, buff);
    UTEST_COMPARE_CSTR("0", buff);
    i32toa(-100, buff);
    UTEST_COMPARE_CSTR("-100", buff);
    uint64_t u64 = std::numeric_limits<uint32_t>::max();
    u64 += std::numeric_limits<uint32_t>::max();
    u64toa(u64, buff);
    UTEST_COMPARE_CSTR("8589934590", buff);

    const int64_t i64 = -1 * u64;
    i64toa(i64, buff);
    UTEST_COMPARE_CSTR("-8589934590", buff);

    return nullptr;
}

} // namespace gmio
