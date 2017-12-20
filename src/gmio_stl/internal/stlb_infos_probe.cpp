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

#include "stlb_infos_probe.h"

#include "../../gmio_core/error.h"
#include "../../gmio_core/internal/byte_swap.h"

#include <cstring>

namespace gmio {

static Endianness STL_binaryFormatEndianness(STL_Format format)
{
    if (format == STL_Format_BinaryBigEndian)
        return Endianness_Big;
    else if (format == STL_Format_BinaryLittleEndian)
        return Endianness_Little;
    return Endianness_Unknown;
}

Result<STL_Infos> STL_probeBinaryInfos(
        FuncReadData func_read,
        unsigned flags,
        STL_ProbeInfosOptions options)
{
    STL_Infos infos = {};
    if (flags != 0) {
        infos.format = options.format_hint;

        const Endianness byte_order =
                STL_binaryFormatEndianness(options.format_hint);
        uint32_t facet_count = 0;
        uint8_t buff[STL_BinaryHeaderSize + sizeof(uint32_t)];

        if (!readExactly(func_read, buff, sizeof(buff)))
            return makeError(Error_Stream);

        std::memcpy(&facet_count, buff + STL_BinaryHeaderSize, sizeof(uint32_t));
        if (byte_order != Endianness_Host)
            facet_count = uint32_byteSwap(facet_count);

        if (flags & STL_InfoFlag_BinaryHeader)
            std::memcpy(infos.binary_header.data(), buff, STL_BinaryHeaderSize);
        if (flags & STL_InfoFlag_FacetCount)
            infos.facet_count = facet_count;
        if (flags & STL_InfoFlag_Size)
            infos.size = STL_binarySize(facet_count);
    }
    return infos;
}

uint64_t STL_binarySize(uint32_t facet_count)
{
    return STL_BinaryHeaderSize
            + sizeof(uint32_t)
            + facet_count * STL_TriangleBinaryRawSize;
}

} // namespace gmio
