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

#include "../iodevice.h"
#include "../span.h"
#include "string_ascii_utils.h"

#include <string>

namespace gmio {

enum CaseSensitivity {
    Case_Sensitive,
    Case_Insensitive
};

class TextIStream {
public:
    TextIStream(Span<char> buff, FuncReadData func_read);
    TextIStream(Span<uint8_t> buff, FuncReadData func_read);

    template<typename OUTPUT_ITERATOR>
    bool eatWord(OUTPUT_ITERATOR out);
    bool eatWord(std::string* str);
    std::string eatWord();

    bool eatExactly(const char* str, CaseSensitivity sens = Case_Sensitive);

    inline unsigned eatUnsignedInt();
    inline int eatInt();
    float eatFloat();

    inline const char* skipAsciiSpaces();
    template<typename OUTPUT_ITERATOR>
    void eatAsciiSpaces(OUTPUT_ITERATOR out);
    void eatAsciiSpaces(std::string* str);

    template<typename FUNC_PREDICATE>
    const char* skipWhile(FUNC_PREDICATE predicate);

    constexpr const char* currentChar() const;
    const char* nextChar();
    constexpr const char* bufferEnd() const;

private:
    float fast_eatFloatImpl(unsigned* ptr_char_diff = nullptr);
    float fast_eatFloat();

    uint64_t readNextChunk();

    FuncReadData m_func_read;
    Span<char> m_buffer;
    const char* m_buffer_at;
    const char* m_buffer_end;
};


// Implementation

// Advances stream so that next word is extracted into 'out'
template<typename OUTPUT_ITERATOR>
bool TextIStream::eatWord(OUTPUT_ITERATOR out)
{
    const char* curr_char = this->skipAsciiSpaces();
    if (curr_char == nullptr) // Empty word
        return false;
    do {
        *out = *curr_char;
        curr_char = this->nextChar();
        ++out;
    } while (curr_char != nullptr && !ascii_isSpace(*curr_char));
    return true;
}

unsigned TextIStream::eatUnsignedInt()
{
    unsigned int value = 0;
    for (const char* in = this->currentChar();
         in != nullptr && ascii_isDigit(*in);
         in = this->nextChar())
    {
        value = (value * 10) + (*in - '0');
    }
    return value;
}

int TextIStream::eatInt()
{
    const char* in = this->currentChar();
    const int sign = *in == '-' ? -1 : 1;
    if (sign == -1 || *in == '+')
        in = this->nextChar();
    return sign * this->eatUnsignedInt();
}

// Returns the char where the stream is currently pointing at
constexpr const char *TextIStream::currentChar() const
{
    return m_buffer_at < m_buffer_end ? m_buffer_at : nullptr;
}

// Advances stream until the first non-space char is met and copies in output
// any space found
template<typename OUTPUT_ITERATOR>
void TextIStream::eatAsciiSpaces(OUTPUT_ITERATOR out)
{
    const char* curr_char = this->currentChar();
    while (curr_char != nullptr && ascii_isSpace(*curr_char)) {
        *out = *curr_char;
        curr_char = this->nextChar();
    }
}

// Advances stream position until the first non-space char is met
const char *TextIStream::skipAsciiSpaces()
{
    return this->skipWhile(ascii_isSpace);
}

template<typename FUNC_PREDICATE>
const char* TextIStream::skipWhile(FUNC_PREDICATE predicate)
{
    const char* curr_char = this->currentChar();
    while (curr_char != nullptr && predicate(*curr_char))
        curr_char = this->nextChar();
    return curr_char;
}

constexpr const char *TextIStream::bufferEnd() const
{
    return m_buffer_end;
}

} // namespace gmio

#if 0

int gmio_get_float32(const char* str, float* value_ptr)
{
#if GMIO_STR2FLOAT_LIB == GMIO_STR2FLOAT_LIB_STD
    char* end_ptr = NULL;
    *value_ptr = std::strtof(str, &end_ptr);
    return (end_ptr == str || errno == ERANGE) ? -1 : 0;
#elif GMIO_STR2FLOAT_LIB == GMIO_STR2FLOAT_LIB_IRRLICHT
    const char* end_ptr = NULL;
    *value_ptr = fast_strtof(str, &end_ptr);
    return end_ptr == str ? -1 : 0;
#elif GMIO_STR2FLOAT_LIB == GMIO_STR2FLOAT_LIB_DOUBLE_CONVERSION
    size_t len = 0;
    while (gmio_ascii_isspace(*str) && *str != '\0')
        ++str;
    while (!gmio_ascii_isspace(*(str + len)) && *(str + len) != '\0')
        ++len;
    *value_ptr = gmio_str2float_googledoubleconversion(str, len);
    return 0;
#endif
}

float gmio_to_float32(const char* str)
{
#if GMIO_STR2FLOAT_LIB == GMIO_STR2FLOAT_LIB_STD
    return std::strtof(str, NULL);
#elif GMIO_STR2FLOAT_LIB == GMIO_STR2FLOAT_LIB_IRRLICHT
    return fast_atof(str);
#elif GMIO_STR2FLOAT_LIB == GMIO_STR2FLOAT_LIB_DOUBLE_CONVERSION
    return GDC_str2float(str, std::strlen(str));
#endif
}

#endif
