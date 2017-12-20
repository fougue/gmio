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

#include "text_istream.h"

#include "small_vector.h"

#include <iterator>

#if GMIO_STR2FLOAT_LIB == GMIO_STR2FLOAT_LIB_IRRLICHT
#  include "fast_atof.h"
#elif GMIO_STR2FLOAT_LIB == GMIO_STR2FLOAT_LIB_DOUBLE_CONVERSION
#  include "google_doubleconversion.h"
#endif

namespace gmio {

namespace {

template<typename FUNC_COMPARE>
bool TextStream_eatExactly(TextIStream* stream, const char* str, FUNC_COMPARE comp)
{
    const char* c = stream->currentChar();
    while (c != nullptr && *str != '\0') {
        if (!comp(*c, *str))
            return false;
        c = stream->nextChar();
        ++str;
    }
    return *str == '\0';
}

} // namespace

// 'func_read' is called each time next contents chunk has to be read
TextIStream::TextIStream(Span<char> buff, FuncReadData func_read)
    : m_buffer(std::move(buff)),
      m_func_read(std::move(func_read))
{
    this->readNextChunk();
}

TextIStream::TextIStream(Span<uint8_t> buff, FuncReadData func_read)
    : TextIStream(makeSpan(reinterpret_cast<char*>(buff.data()), buff.size()),
                  func_read)
{
}

bool TextIStream::eatWord(std::string *str)
{
    return this->eatWord(std::back_inserter(*str));
}

std::string TextIStream::eatWord()
{
    std::string str;
    this->eatWord(std::back_inserter(str));
    return str;
}

// Eats 'str' and stops on any difference
bool TextIStream::eatExactly(const char *str, CaseSensitivity sens)
{
    if (sens == Case_Sensitive)
        return TextStream_eatExactly(this, str, std::equal_to<char>());
    else
        return TextStream_eatExactly(this, str, ascii_iequals);
}

void TextIStream::eatAsciiSpaces(std::string *str)
{
    this->eatAsciiSpaces(std::back_inserter(*str));
}

// Moves on next char in stream
const char *TextIStream::nextChar()
{
    ++m_buffer_at;
    if (m_buffer_at < m_buffer_end)
        return m_buffer_at;
    return this->readNextChunk() > 0 ? m_buffer_at : nullptr;
}

namespace {

constexpr bool fast_isLocalDecimalPoint(char in)
{
    // Selection of characters which count as decimal point in fast_atof
    // TODO: This should probably also be used in irr::core::string, but
    // the float-to-string code used there has to be rewritten first
    return in == '.';
}

const float fast_atof_table[] = {
    0.f,
    0.1f,
    0.01f,
    0.001f,
    0.0001f,
    0.00001f,
    0.000001f,
    0.0000001f,
    0.00000001f,
    0.000000001f,
    0.0000000001f,
    0.00000000001f,
    0.000000000001f,
    0.0000000000001f,
    0.00000000000001f,
    0.000000000000001f,
    0.0000000000000001f
};

} // namespace

float TextIStream::eatFloat()
{
#if GMIO_STR2FLOAT_LIB == GMIO_STR2FLOAT_LIB_STD
    SmallVector<char, 32> num;
    this->eatWord(std::back_inserter(num));
    return static_cast<float>(std::atof(num.data()));
#elif GMIO_STR2FLOAT_LIB == GMIO_STR2FLOAT_LIB_IRRLICHT
    return this->fast_eatFloat();
#elif GMIO_STR2FLOAT_LIB == GMIO_STR2FLOAT_LIB_DOUBLE_CONVERSION
    SmallVector<char, 32> num;
    this->eatWord(std::back_inserter(num));
    return GDC_str2float(num.data(), num.size());
#endif
}

float TextIStream::fast_eatFloatImpl(unsigned *ptr_char_diff)
{
    static const uint32_t max_safe_u32_value = UINT_MAX / 10 - 10;
    uint32_t uval = 0;
    unsigned char_diff = 0;

    // Use integer arithmetic for as long as possible, for speed and precision
    for (const char* in = this->currentChar();
         in != nullptr && ascii_isDigit(*in) && uval < max_safe_u32_value;
         in = this->nextChar())
    {
        uval = (uval * 10) + (*in - '0');
        ++char_diff;
    }
    float fval = static_cast<float>(uval);
    // If there are any digits left to parse, then we need to use floating point
    // arithmetic from here
    for (const char* in = this->currentChar();
         in != nullptr && ascii_isDigit(*in) && fval <= FLT_MAX;
         in = this->nextChar())
    {
        fval = (fval * 10) + (*in - '0');
        ++char_diff;
    }
    if (ptr_char_diff != nullptr)
        *ptr_char_diff = char_diff;
    return fval;
}

float TextIStream::fast_eatFloat()
{
    const char* in = this->currentChar();
    const int sign = *in == '-' ? -1 : 1;

    // Please run the regression test when making any modifications to this
    // function
    if (sign == -1 || ('+' == *in))
        in = this->nextChar();
    float value = this->fast_eatFloatImpl();
    in = this->currentChar();
    if (fast_isLocalDecimalPoint(*in)) {
        this->nextChar();
        unsigned char_diff;
        const float fdecimal = this->fast_eatFloatImpl(&char_diff);
        value += fdecimal * fast_atof_table[char_diff];
        in = this->currentChar();
    }
    if (in != nullptr && ascii_iequals(*in, 'e')) {
        in = this->nextChar();
        // Assume that the exponent is a whole number.
        // strtol10() will deal with both + and - signs,
        // but calculate as float to prevent overflow at FLT_MAX
        value *= std::pow(10.f, static_cast<float>(this->eatInt()));
    }
    return sign * value;
}

uint64_t TextIStream::readNextChunk()
{
    const uint64_t readlen = m_func_read(
                reinterpret_cast<uint8_t*>(m_buffer.data()),
                m_buffer.size(),
                nullptr);
    m_buffer_end = readlen > 0 ? m_buffer.data() + readlen : nullptr;
    m_buffer_at = readlen > 0 ? m_buffer.data() : nullptr;
    return readlen;
}

} // namespace gmio
