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

#include "stl_io.h"

#include "stl_error.h"
#include "internal/stl_error_check.h"
#include "internal/stlb_byte_swap.h"

#include "../gmio_core/endian.h"
#include "../gmio_core/error.h"
#include "../gmio_core/internal/byte_swap.h"
#include "../gmio_core/internal/buffer_read_helper.h"
#include "../gmio_core/internal/io_buffer_helper.h"
#include "../gmio_core/internal/safe_cast.h"
#include "../gmio_core/internal/scoped_resources.h"

#include <algorithm>
#include <cstring>
#include <utility>

namespace gmio {

namespace {

inline STL_Triangle STL_decodeFacet(const uint8_t* buffer)
{
    STL_Triangle triangle;
    // triangle = *((STL_Triangle*)(buffer));
    std::memcpy(&triangle, buffer, STL_TriangleBinaryRawSize);
    return triangle;
}

void STL_decodeFacets(
        STL_MeshCreator* creator, const uint8_t* buffer, uint32_t count, uint32_t offset)
{
    for (uint32_t i = 0; i < count; ++i) {
        const STL_Triangle triangle = STL_decodeFacet(buffer);
        buffer += STL_TriangleBinaryRawSize;
        creator->addTriangle(offset + i, triangle);
    }
}

void STL_decodeFacetsByteSwap(
        STL_MeshCreator* creator, const uint8_t* buffer, uint32_t count, uint32_t offset)
{
    for (uint32_t i = 0; i < count; ++i) {
        STL_Triangle triangle = STL_decodeFacet(buffer);
        buffer += STL_TriangleBinaryRawSize;
        STL_byteSwapTriangle(&triangle);
        creator->addTriangle(offset + i, triangle);
    }
}

Span<uint8_t> bufferInitialData(const STL_ReadOptions& options)
{
    return makeSpan(options.buffer.data(), options.buffer_initial_contents_size);
}

} // namespace

int STL_readBinary(
        Endianness byte_order,
        FuncReadData func_read,
        STL_MeshCreator* creator,
        STL_ReadOptions options)
{
    Task* task = options.task;
    const IoBufferHelper buffer_helper(options.buffer);
    Span<uint8_t> buffer = buffer_helper.get();
    STL_MeshCreatorInfos infos = {};
    const auto func_decode_facets =
            byte_order != Endianness_Host ?
                STL_decodeFacetsByteSwap :
                STL_decodeFacets;
    const uint32_t max_facet_count_per_read =
            size_to_uint32(buffer.size() / STL_TriangleBinaryRawSize);

    options.buffer = buffer;
    ScopedResources scoped_res;
    if (options.buffer_initial_contents_size > 0) {
        auto initial_data = bufferInitialData(options);
        auto helper = scoped_res.emplace<BufferReadHelper>(func_read, initial_data);
        func_read = BufferReadHelper_funcReadData(helper);
    }

    // Check validity of input parameters
    if (!STL_binaryCheckByteOrder(byte_order))
        return STL_Error_UnsupportedByteOrder;

    // Read header
    if (!readExactly(func_read, &infos.binary_header))
        return STL_Error_HeaderWrongSize;

    // Read facet count
    if (!readExactly(func_read, &infos.binary_triangle_count, 1))
        return STL_Error_FacetCount;
    if (byte_order != Endianness_Host)
        infos.binary_triangle_count = uint32_byteSwap(infos.binary_triangle_count);

    // Callback to notify triangle count and header data
    infos.format =
            byte_order == Endianness_Little ?
                STL_Format_BinaryLittleEndian :
                STL_Format_BinaryBigEndian;
    creator->beginSolid(infos);

    // Read triangles
    if (task != nullptr)
        task->handleProgress(0, infos.binary_triangle_count);
    uint32_t i_facet = 0;
    while (i_facet < infos.binary_triangle_count) {
        ReadState read_state;
        const uint32_t facet_count_to_read =
                std::min(max_facet_count_per_read,
                         infos.binary_triangle_count - i_facet);
        const uint64_t size_to_read =
                STL_TriangleBinaryRawSize * facet_count_to_read;
        const uint64_t size_actual_read =
                func_read(buffer.data(), size_to_read, &read_state);
        const uint32_t read_facet_count =
                size_actual_read / STL_TriangleBinaryRawSize;
        if (read_state == ReadState::Error)
            return Error_Stream;
        else if (read_facet_count == 0)
            break; // Exit if no facet to read

        func_decode_facets(creator, buffer.data(), read_facet_count, i_facet);
        i_facet += read_facet_count;
        if (task != nullptr) {
            if (task->isStopRequested())
                return Error_TaskStopped;
            task->handleProgress(i_facet, infos.binary_triangle_count);
        }
    } // end while

    creator->endSolid();
    if (i_facet != infos.binary_triangle_count)
        return STL_Error_FacetCount;
#if 0
    // Try to eat EOF by probing next byte in stream
    if (!device->atEnd()) {
        const IoDevice::Pos oldpos = device->readPos();
        uint8_t c;
        device->read(&c, 1);
        if (!device->atEnd())
            device->setReadPos(oldpos);
    }
#endif

    return Error_OK;
}

} // namespace gmio
