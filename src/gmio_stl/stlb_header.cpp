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

#include "stlb_header.h"

#include <algorithm>
#include <cctype>
#include "../../gmio_core/internal/string_ascii_utils.h"

namespace gmio {

std::array<char, STL_BinaryHeaderSize+1> STL_binaryHeaderToString(
        const STL_BinaryHeader& header, char replacement)
{
    std::array<char, STL_BinaryHeaderSize+1> str;
    for (size_t i = 0; i < header.size(); ++i) {
        const auto hc = static_cast<char>(header.at(i));
        const auto hi = static_cast<int>(header.at(i));
        str[i] = std::isprint(hi) ? hc : replacement;
    }
    str[STL_BinaryHeaderSize] = '\0';
    ascii_trimEnd(str.data(), str.size());
    return str;
}

STL_BinaryHeader STL_binaryHeaderFromString(Span<const char> str)
{
    STL_BinaryHeader header = {};
    const size_t copyLen = std::min(header.size(), str.size());
    std::strncpy(reinterpret_cast<char*>(header.data()), str.data(), copyLen);
    return header;
}

} // namespace gmio
