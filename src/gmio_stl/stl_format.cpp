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

#include "stl_format.h"

#include "stl_triangle.h"
#include "stlb_header.h"
#include "internal/stlb_byte_swap.h"
#include "internal/stlb_infos_probe.h"

#include "../gmio_core/endian.h"
#include "../gmio_core/iodevice.h"
#include "../gmio_core/internal/byte_codec.h"
#include "../gmio_core/internal/byte_swap.h"
#include "../gmio_core/internal/numeric_utils.h"
#include "../gmio_core/internal/string_ascii_utils.h"
#include "../gmio_core/internal/vecgeom_utils.h"

#include <cstring>

namespace gmio {

namespace {

inline bool ascii_strHasToken(const char* str, const char* token)
{
    const char* substr = ascii_istrstr(str, token);
    return substr != nullptr && ascii_isSpace(*(substr - 1));
}

STL_Format STL_binaryFormat(Span<const uint8_t> bytes, uint64_t hint_solid_size)
{
    static const size_t triangle_offset = STL_BinaryHeaderSize + 4;
    if (bytes.size() >= triangle_offset) {
        const uint32_t le_facet_count =
                uint32_decodeLittleEndian(bytes.data() + STL_BinaryHeaderSize);
        const uint32_t be_facet_count =
                uint32_byteSwap(le_facet_count);

        if (hint_solid_size != 0) {
            if (STL_binarySize(le_facet_count) == hint_solid_size)
                return STL_Format_BinaryLittleEndian;
            if (STL_binarySize(be_facet_count) == hint_solid_size)
                return STL_Format_BinaryBigEndian;
        }
        else if (le_facet_count == 0 && bytes.size() == triangle_offset) {
            return STL_Format_BinaryLittleEndian;
        }

        // Investigate to check if first facet's normal has length ~1.f
        if ((le_facet_count > 0 || be_facet_count > 0)
                && bytes.size() >= (triangle_offset + STL_TriangleBinaryRawSize))
        {
            STL_Triangle tri;
            std::memcpy(&tri,
                        bytes.data() + triangle_offset,
                        STL_TriangleBinaryRawSize);
            if (float32_ulpEquals(vec3_sqrLength(tri.n), 1.f, 100)) {
                return Endianness_Host == Endianness_Little ?
                            STL_Format_BinaryLittleEndian :
                            STL_Format_BinaryBigEndian;
            }
            STL_byteSwapTriangle(&tri);
            if (float32_ulpEquals(vec3_sqrLength(tri.n), 1.f, 100)) {
                return Endianness_Host == Endianness_Little ?
                            STL_Format_BinaryBigEndian :
                            STL_Format_BinaryLittleEndian;
            }
        }
    }

    return STL_Format_Unknown;
}

bool STL_isAsciiFormat(Span<const uint8_t> bytes)
{
    const auto str = reinterpret_cast<const char*>(bytes.data());
    const size_t str_len = bytes.size();

    // Skip spaces at beginning
    size_t pos = 0;
    while (pos < str_len && ascii_isSpace(str[pos]))
        ++pos;

    // Next token (if exists) must match "solid\s"
    if ((pos + 6) < str_len
            && ascii_istartsWith(str + pos, "solid")
            && ascii_isSpace(str[pos + 5]))
    {
        // Try to find some STL ascii keyword
        pos += 6;
        return ascii_strHasToken(str + pos, "facet")
                || ascii_strHasToken(str + pos, "endsolid");
    }
    return false;
}

} // namespace

STL_Format STL_probeFormat(Span<const uint8_t> bytes, uint64_t hint_solid_size)
{
    const STL_Format binaryFormat = STL_binaryFormat(bytes, hint_solid_size);
    if (binaryFormat != STL_Format_Unknown)
        return binaryFormat;
    if (STL_isAsciiFormat(bytes))
        return STL_Format_Ascii;
    return STL_Format_Unknown;
}

STL_Format STL_probeFormat(const char* filepath)
{
    FILE* file = std::fopen(filepath, "rb");
    if (file != nullptr) {
        uint8_t buff[512];
        const size_t read_size = std::fread(buff, 1, sizeof(buff), file);
        const uint64_t file_size = FILE_size(file);
        std::fclose(file);
        return STL_probeFormat(makeSpan(buff, read_size), file_size);
    }
    return STL_Format_Unknown;
}

} // namespace gmio
