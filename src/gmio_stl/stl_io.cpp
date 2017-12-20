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
#include "internal/stla_write.h"
#include "internal/stlb_write.h"
#include "../gmio_core/error.h"
#include "../gmio_core/iodevice.h"
#include "../gmio_core/internal/byte_codec.h"
#include "../gmio_core/internal/io_buffer_helper.h"

#include <algorithm>

namespace gmio {

int STL_read(
        FuncReadData func_read,
        STL_MeshCreator* creator,
        STL_ReadOptions options)
{
    // Make sure there is a usable buffer for reading
    const IoBufferHelper buffer_helper(options.buffer);
    Span<uint8_t> buffer = buffer_helper.get();

    // Probe format
    const size_t probe_size = std::min<size_t>(512, buffer.size());
    ReadState read_state;
    const uint64_t read_size = func_read(buffer.data(), probe_size, &read_state);
    if (read_size == 0 && read_state == ReadState::Eof)
        return Error_OK;
    const STL_Format format = STL_probeFormat(makeSpan(buffer.data(), read_size));

    // Update the ReadOptions object to refer to the local buffer
    options.buffer = buffer_helper.get();
    options.buffer_initial_contents_size = read_size;

    // Dispatch
    switch (format) {
    case STL_Format_Ascii:
        return STL_readAscii(func_read, creator, options);
    case STL_Format_BinaryBigEndian:
        return STL_readBinary(Endianness_Big, func_read, creator, options);
    case STL_Format_BinaryLittleEndian:
        return STL_readBinary(Endianness_Little, func_read, creator, options);
    case STL_Format_Unknown:
        return STL_Error_UnknownFormat;
    }
    return Error_Unknown;
}

int STL_read(
        const char* filepath,
        STL_MeshCreator* creator,
        STL_ReadOptions options)
{
    FILE* file = std::fopen(filepath, "rb");
    if (file != nullptr) {
        const int error = STL_read(FILE_funcReadData(file), creator, options);
        std::fclose(file);
        return error;
    }
    return Error_Stdio;
}

int STL_write(
        STL_Format format,
        FuncWriteData func_write,
        const STL_Mesh& mesh,
        STL_WriteOptions options)
{
    switch (format) {
    case STL_Format_Ascii:
        return STL_writeAscii(func_write, mesh, options);
    case STL_Format_BinaryBigEndian:
        return STL_writeBinary(Endianness_Big, func_write, mesh, options);
    case STL_Format_BinaryLittleEndian:
        return STL_writeBinary(Endianness_Little, func_write, mesh, options);
    case STL_Format_Unknown:
        return STL_Error_UnknownFormat;
    }
    return Error_Unknown;
}

int STL_write(
        STL_Format format,
        const char* filepath,
        const STL_Mesh& mesh,
        STL_WriteOptions options)
{
    FILE* file = std::fopen(filepath, "wb");
    if (file != nullptr) {
        const int error =
                STL_write(format, FILE_funcWriteData(file), mesh, options);
        std::fclose(file);
        return error;
    }
    return Error_Stdio;
}

int STL_writeBinaryHeader(
        Endianness byte_order,
        FuncWriteData func_write,
        const STL_BinaryHeader& header,
        uint32_t facet_count)
{
    // Write 80-byte header
    if (!writeExactly(func_write, header))
        return Error_Stream;

    // Write facet count
    std::array<uint8_t, sizeof(uint32_t)> facet_count_bytes;
    if (byte_order == Endianness_Little)
        uint32_encodeLittleEndian(facet_count, facet_count_bytes.data());
    else if (byte_order == Endianness_Big)
        uint32_encodeBigEndian(facet_count, facet_count_bytes.data());
    else
        return STL_Error_UnsupportedByteOrder;
    if (!writeExactly(func_write, facet_count_bytes))
        return Error_Stream;

    return Error_OK;
}

} // namespace gmio
