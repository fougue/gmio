/****************************************************************************
** Copyright (c) 2016, Fougue Ltd. <http://www.fougue.pro>
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

#include "ostringstream.h"

#include "helper_stream.h"
#include "itoa.h"
#include "min_max.h"
#include "../../3rdparty/base64/b64.h"

#if GMIO_FLOAT2STR_LIB == GMIO_FLOAT2STR_LIB_STD
#  include "c99_stdio_compat.h"
#elif GMIO_FLOAT2STR_LIB == GMIO_FLOAT2STR_LIB_DOUBLE_CONVERSION
#  include "google_doubleconversion.h"
#endif

#include <string.h>

GMIO_INLINE char* gmio_ostringstream_strbuff_pos(
        const struct gmio_ostringstream* sstream)
{
    return sstream->strbuff.ptr + sstream->strbuff.len;
}

GMIO_INLINE size_t gmio_string_remaining_capacity(const struct gmio_string* str)
{
    return str->capacity - str->len;
}

struct gmio_ostringstream gmio_ostringstream(
        const struct gmio_stream stream, const struct gmio_string strbuff)
{
    struct gmio_ostringstream sstream = {0};
    sstream.stream = stream;
    sstream.strbuff = strbuff;
    sstream.func_stream_write = gmio_ostringstream_default_func_write;
    return sstream;
}


size_t gmio_ostringstream_default_func_write(
        void* cookie, struct gmio_stream* stream, const char* ptr, size_t len)
{
    GMIO_UNUSED(cookie);
    return gmio_stream_write_bytes(stream, ptr, len);
}

void gmio_ostringstream_write_str(
        struct gmio_ostringstream *sstream, const char *str)
{
    gmio_ostringstream_write_nstr(sstream, str, strlen(str));
}

void gmio_ostringstream_write_nstr(
        struct gmio_ostringstream *sstream, const char *str, size_t n)
{
    struct gmio_string* strbuff = &sstream->strbuff;
    if (n > strbuff->capacity) {
        /* No need to bufferize copy */
        gmio_ostringstream_flush(sstream);
        sstream->func_stream_write(sstream->cookie, &sstream->stream, str, n);
    }
    else {
        /* str fits in sstream->strbuff */
        if (n >= gmio_string_remaining_capacity(strbuff))
            gmio_ostringstream_flush(sstream);
        strncpy(gmio_ostringstream_strbuff_pos(sstream), str, n);
        strbuff->len += n;
    }
}

void gmio_ostringstream_write_xmlattr_str(
        struct gmio_ostringstream *sstream, const char *attr, const char *val)
{
    gmio_ostringstream_write_char(sstream, ' ');
    gmio_ostringstream_write_str(sstream, attr);
    gmio_ostringstream_write_chararray(sstream, "=\"");
    gmio_ostringstream_write_str(sstream, val);
    gmio_ostringstream_write_char(sstream, '\"');
}

void gmio_ostringstream_write_xmlattr_u32(
        struct gmio_ostringstream *sstream, const char *attr, uint32_t val)
{
    gmio_ostringstream_write_char(sstream, ' ');
    gmio_ostringstream_write_str(sstream, attr);
    gmio_ostringstream_write_chararray(sstream, "=\"");
    gmio_ostringstream_write_u32(sstream, val);
    gmio_ostringstream_write_char(sstream, '\"');
}

void gmio_ostringstream_write_u32(
        struct gmio_ostringstream *sstream, uint32_t value)
{
    struct gmio_string* buff = &sstream->strbuff;
    char* buffpos;
    const char* newbuffpos;
    if (gmio_string_remaining_capacity(buff) < 10)
        gmio_ostringstream_flush(sstream);
    buffpos = buff->ptr + buff->len;
    newbuffpos = gmio_u32toa(value, buffpos);
    buff->len += newbuffpos - buffpos;
}

void gmio_ostringstream_write_i32(
        struct gmio_ostringstream *sstream, int32_t value)
{
    struct gmio_string* buff = &sstream->strbuff;
    char* buffpos;
    const char* newbuffpos;
    if (gmio_string_remaining_capacity(buff) < 11) /* 10 digits + '-' sign */
        gmio_ostringstream_flush(sstream);
    buffpos = buff->ptr + buff->len;
    newbuffpos = gmio_i32toa(value, buffpos);
    buff->len += newbuffpos - buffpos;
}

void gmio_ostringstream_write_xmlelt_f64(
        struct gmio_ostringstream *sstream, const char *elt, double val)
{
    const size_t elt_strlen = strlen(elt);
    gmio_ostringstream_write_char(sstream, '<');
    gmio_ostringstream_write_nstr(sstream, elt, elt_strlen);
    gmio_ostringstream_write_char(sstream, '>');
    gmio_ostringstream_write_f64(sstream, val, NULL);
    gmio_ostringstream_write_nstr(sstream, "</", 2);
    gmio_ostringstream_write_nstr(sstream, elt, elt_strlen);
    gmio_ostringstream_write_char(sstream, '>');
}

void gmio_ostringstream_write_f64(
        struct gmio_ostringstream *sstream,
        double value,
        const struct gmio_ostringstream_format_float *format)
{
    static const struct gmio_ostringstream_format_float default_format = {
        "%.16g", GMIO_FLOAT_TEXT_FORMAT_SHORTEST_UPPERCASE, 16 };
    const struct gmio_ostringstream_format_float* nformat =
            format != NULL ? format : &default_format;
    struct gmio_string* buff = &sstream->strbuff;
    int written_count = 0;
    if (buff->capacity - buff->len < 32)
        gmio_ostringstream_flush(sstream);
#if GMIO_FLOAT2STR_LIB == GMIO_FLOAT2STR_LIB_STD
    written_count = gmio_snprintf(
                buff->ptr + buff->len,
                buff->capacity - buff->len,
                nformat->printf_format,
                value);
#elif GMIO_FLOAT2STR_LIB == GMIO_FLOAT2STR_LIB_DOUBLE_CONVERSION
    written_count = gmio_double2str_googledoubleconversion(
                value,
                buff->ptr + buff->len,
                buff->capacity - buff->len,
                nformat->text_format,
                nformat->precision);
#endif
    buff->len += written_count;
}

void gmio_ostringstream_write_base64(
        struct gmio_ostringstream *sstream,
        const unsigned char *input,
        size_t len)
{
    struct gmio_string* buff = &sstream->strbuff;
    size_t pos = 0;
    while (pos < len) {
        const size_t remaining_len = len - pos;
        unsigned char* output;
        if (gmio_string_remaining_capacity(buff) < 4)
            gmio_ostringstream_flush(sstream);
        output = (unsigned char*)(buff->ptr + buff->len);
        if (remaining_len >= 3) {
            b64_encodeblock(input + pos, output, 3);
        }
        else {
            unsigned char temp_in[3] = {0};
            if (remaining_len > 0)
                temp_in[0] = *(input + pos);
            if (remaining_len > 1)
                temp_in[1] = *(input + pos + 1);
            b64_encodeblock(temp_in, output, remaining_len);
        }
        pos += 3;
        buff->len += 4;
    }
}
