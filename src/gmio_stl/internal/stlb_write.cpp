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

#include "stlb_write.h"

#include "stl_error_check.h"
#include "stlb_byte_swap.h"
#include "../stl_error.h"
#include "../stl_io.h"
#include "../stl_io_options.h"

#include "../../gmio_core/error.h"
#include "../../gmio_core/internal/byte_codec.h"
#include "../../gmio_core/internal/io_buffer_helper.h"
#include "../../gmio_core/internal/safe_cast.h"

#include <algorithm>
#include <cstring>

namespace gmio {

namespace {

inline void STL_encodeFacet(const STL_Triangle& triangle, uint8_t* buffer)
{
    std::memcpy(buffer, &triangle, STL_TriangleBinaryRawSize);
}

void STL_encodeFacets(
        const STL_Mesh& mesh, uint8_t* buffer, uint32_t count, uint32_t offset)
{
    for (uint32_t i = 0; i < count; ++i) {
        const STL_Triangle triangle = mesh.triangle(offset + i);
        STL_encodeFacet(triangle, buffer);
        buffer += STL_TriangleBinaryRawSize;
    }
}

void STL_encodeFacetsByteSwap(
        const STL_Mesh& mesh, uint8_t* buffer, uint32_t count, uint32_t offset)
{
    for (uint32_t i = 0; i < count; ++i) {
        STL_Triangle triangle = mesh.triangle(offset + i);
        STL_byteSwapTriangle(&triangle);
        STL_encodeFacet(triangle, buffer);
        buffer += STL_TriangleBinaryRawSize;
    }
}

} // namespace

int STL_writeBinary(
        Endianness byte_order,
        FuncWriteData func_write,
        const STL_Mesh& mesh,
        STL_WriteOptions options)
{
    Task* task = options.task;
    const IoBufferHelper buffer_helper(options.buffer);
    Span<uint8_t> buffer = buffer_helper.get();
    const uint32_t facet_count = mesh.triangleCount();
    const auto func_encode_facets =
            byte_order != Endianness_Host ?
                STL_encodeFacetsByteSwap :
                STL_encodeFacets;

    if (!STL_binaryCheckByteOrder(byte_order))
        return STL_Error_UnsupportedByteOrder;

    if (!options.write_triangles_only) {
        const int error = STL_writeBinaryHeader(
                    byte_order, func_write, options.binary_header, facet_count);
        if (error != Error_OK)
            return error;
    }

    uint32_t write_facet_count = buffer.size() / STL_TriangleBinaryRawSize;
    for (uint32_t i = 0; i < facet_count; i += write_facet_count) {
        if (task != nullptr)
            task->handleProgress(i, facet_count);
        write_facet_count = std::min(write_facet_count, facet_count - i);
        func_encode_facets(mesh, buffer.data(), write_facet_count, i);
        const uint64_t len = STL_TriangleBinaryRawSize * write_facet_count;
        if (func_write(buffer.data(), len) != len)
            return Error_Stream;
        if (task != nullptr && task->isStopRequested())
            return Error_TaskStopped;
    }

    return Error_OK;
}

} // namespace gmio
