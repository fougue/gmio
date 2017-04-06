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

#include "float_format_utils.h"

#include <stdio.h>

char gmio_float_text_format_to_stdio_specifier(
        enum gmio_float_text_format format)
{
    switch (format) {
    case GMIO_FLOAT_TEXT_FORMAT_DECIMAL_LOWERCASE: return 'f';
    case GMIO_FLOAT_TEXT_FORMAT_DECIMAL_UPPERCASE: return 'F';
    case GMIO_FLOAT_TEXT_FORMAT_SCIENTIFIC_LOWERCASE: return 'e';
    case GMIO_FLOAT_TEXT_FORMAT_SCIENTIFIC_UPPERCASE: return 'E';
    case GMIO_FLOAT_TEXT_FORMAT_SHORTEST_LOWERCASE: return 'g';
    case GMIO_FLOAT_TEXT_FORMAT_SHORTEST_UPPERCASE: return 'G';
    }
    /* Default, should not be here */
    return GMIO_FLOAT_TEXT_FORMAT_DECIMAL_LOWERCASE;
}

char* gmio_write_stdio_float_format(char* buffer, char specifier, uint8_t prec)
{
    int prec_len = 0;

    buffer[0] = '%';
    buffer[1] = '.';
    prec_len = sprintf(buffer + 2, "%u", prec);
    buffer[2 + prec_len] = specifier;
    return buffer + 3 + prec_len;
}

struct gmio_string_16 gmio_to_stdio_float_format(
        enum gmio_float_text_format format, uint8_t prec)
{
    struct gmio_string_16 buff = {0};
    const char spec = gmio_float_text_format_to_stdio_specifier(format);
    gmio_write_stdio_float_format(buff.array, spec, prec);
    return buff;
}
