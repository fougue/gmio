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

#pragma once

#if 0

#include "../global.h"
#include "../iodevice.h"
#include "../text_format.h"
#include "string.h"

#include <functional>

namespace gmio {

class TextStream {
public:
    TextStream(IoDevice* device);

private:
    IoDevice* m_device;
};

/*! Output stream that operates on a string
 *
 *  To be used with API below
 */
struct gmio_ostringstream
{
    /*! Stream to iterate over */
    struct gmio_stream stream;

    /*! Holds contents chunk to write in stream(when full) */
    struct gmio_string strbuff;

    /*! Data to be passed to callback func_stream_write */
    void* cookie;

    /*! Pointer on a function called each time next contents chunk has to be
     * written */
    size_t (*func_stream_write)(
            void* cookie, struct gmio_stream* stream, const char* ptr, size_t len);
};

/*! Returns an initialized ostringstream */
struct gmio_ostringstream gmio_ostringstream(
        const struct gmio_stream stream,
        const struct gmio_string strbuff);

/*! Default function for gmio_ostringstream::func_stream_write */
size_t gmio_ostringstream_default_func_write(
        void* cookie, struct gmio_stream* stream, const char* ptr, size_t len);

/*! Writes null-terminated C string \p str to ostringstream \p sstream */
void gmio_ostringstream_write_str(
        struct gmio_ostringstream* sstream, const char* str);

/*! Writes the first \p n characters from C string \p str to ostringstream
 *  \p sstream */
void gmio_ostringstream_write_nstr(
        struct gmio_ostringstream* sstream, const char* str, size_t n);

/*! Writes the character \p array to ostringstream \p sstream
 *
 *  It is a facility around gmio_ostringstream_write_nstr() for string literals
 *  and <tt>char[]</tt>
 *  \code
 *      const char str[] = "stuff";
 *      gmio_ostringstream_write_chararray(sstream, "something");
 *      gmio_ostringstream_write_chararray(sstream, str);
 *  \endcode
 */
#define gmio_ostringstream_write_chararray(sstream, array) \
    gmio_ostringstream_write_nstr((sstream), (array), sizeof(array) - 1)

/*! Writes character \p c to ostringstream \p sstream */
inline void gmio_ostringstream_write_char(
        struct gmio_ostringstream* sstream, char c);

/*! Writes uint32 \p value to ostringstream \p sstream */
void gmio_ostringstream_write_u32(
        struct gmio_ostringstream* sstream, uint32_t value);

/*! Writes int32 \p value to ostringstream \p sstream */
void gmio_ostringstream_write_i32(
        struct gmio_ostringstream* sstream, int32_t value);

/*! Format specification for float numbers(single or double precision) */
struct gmio_ostringstream_format_float
{
    const char* printf_format; /* printf-like format(eg. "%.7f") */
    enum gmio_float_text_format text_format;
    uint8_t precision;
};

/*! Writes double \p value to ostringstream \p sstream */
void gmio_ostringstream_write_f64(
        struct gmio_ostringstream* sstream,
        double value,
        const struct gmio_ostringstream_format_float* format);

/*! Writes \p input encoded to base64 */
void gmio_ostringstream_write_base64(
        struct gmio_ostringstream* sstream,
        const unsigned char* input,
        size_t len);

/*! Write pending bytes within sstream->strbuff */
inline void gmio_ostringstream_flush(struct gmio_ostringstream* sstream);

/* XML */

/*! Writes XML attribute of name \p attr and value \p val */
void gmio_ostringstream_write_xmlattr_str(
        struct gmio_ostringstream* sstream, const char* attr, const char* val);
void gmio_ostringstream_write_xmlattr_u32(
        struct gmio_ostringstream* sstream, const char* attr, uint32_t val);

/*! Writes XML element containing \p val : <elt>val</elt> */
void gmio_ostringstream_write_xmlelt_f64(
        struct gmio_ostringstream* sstream, const char* elt, double val);


inline void gmio_ostringstream_write_xmlcdata_open(
        struct gmio_ostringstream* sstream);
inline void gmio_ostringstream_write_xmlcdata_close(
        struct gmio_ostringstream* sstream);


/*
 * -- Implementation
 */

void gmio_ostringstream_write_char(
        struct gmio_ostringstream* sstream, char c)
{
    struct gmio_string* strbuff = &sstream->strbuff;
    if (strbuff->len >= strbuff->capacity)
        gmio_ostringstream_flush(sstream);
    strbuff->ptr[strbuff->len] = c;
    ++strbuff->len;
}

void gmio_ostringstream_flush(struct gmio_ostringstream* sstream)
{
    struct gmio_string* strbuff = &sstream->strbuff;
    if (strbuff->len > 0) {
        /* Don't check bytecount returned is != strbuff->len because some
         * write() functions may compress data */
        sstream->func_stream_write(
                    sstream->cookie, &sstream->stream, strbuff->ptr, strbuff->len);
        strbuff->len = 0;
    }
}

void gmio_ostringstream_write_xmlcdata_open(struct gmio_ostringstream *sstream)
{
    gmio_ostringstream_write_chararray(sstream, "<![CDATA[");
}

void gmio_ostringstream_write_xmlcdata_close(struct gmio_ostringstream *sstream)
{
    gmio_ostringstream_write_chararray(sstream, "]]>");
}

} // namespace gmio

#endif
