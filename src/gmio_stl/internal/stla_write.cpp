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

#include "stla_write.h"

#include "stl_error_check.h"
#include "../stl_error.h"

#include "../../gmio_core/error.h"
#include "../../gmio_core/span.h"
#include "../../gmio_core/task.h"
#include "../../gmio_core/text_format.h"
#include "../../gmio_core/internal/float_format_utils.h"
#include "../../gmio_core/internal/io_buffer_helper.h"
#include "../../gmio_core/internal/locale_utils.h"
#include "../../gmio_core/internal/safe_cast.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <vector>

#if GMIO_FLOAT2STR_LIB == GMIO_FLOAT2STR_LIB_DOUBLE_CONVERSION
#  include "../../gmio_core/internal/google_doubleconversion.h"
#endif

//
// |     14     ||      17       ||      17       ||     15      |  -> 73
// facet normal  0.986544556E+00  0.986544556E+00  0.986544556E+00
// |   11    |
//  outer loop
// |   10   ||      17       ||      17       ||     15      |      -> 69
//   vertex  0.167500112E+02  0.505000112E+02  0.000000000E+00
//   vertex  0.164599000E+02  0.505000111E+02  0.221480112E+01
//   vertex  0.166819000E+02  0.483135112E+02  0.221480112E+01
// |  8   |
//  endloop
// |  8   |
// endfacet
//
// Total without EOL = 73 + 11 + 3*69 + 8 + 8 = 307
// Total with EOL(2 chars) = 307 + 7*2 = 321
//

namespace gmio {

namespace {

const unsigned STL_AsciiFacetSize = 321;
const unsigned STL_AsciiFacetSizeP2 = 512; // Aligned to closest power of two

// Fucntions for raw strings(ie. "const char*")

inline char* copyChar(char* buffer, char c) {
    *buffer = c;
    return buffer + 1;
}

inline char* copyEol(char* buffer) {
    return copyChar(buffer, '\n');
}

inline char* copyString(char* buffer, const char* str, size_t len) {
    std::strncpy(buffer, str, len);
    return buffer + len;
}

template<size_t N>
inline char* copyString(char* buffer, const char (&str)[N]) {
    return copyString(buffer, &str[0], N - 1);
}

inline char* copyString(char* buffer, const std::string& str) {
    return copyString(buffer, str.data(), str.size());
}

struct TextFormatVec3f {
    FloatTextFormat coord_format;
    uint8_t coord_prec;
    std::array<char, 32> str_printf_format; // printf-like format for XYZ coords
};

std::array<char, 32> coordsPrintfFormat(uint8_t prec, FloatTextFormat format)
{
    std::array<char, 32> txtformat;
    const char spec = toStdioSpecifier(format);
    char* buffpos = txtformat.data();
    buffpos = copyStdioFloatFormat(buffpos, spec, prec);
    buffpos = copyChar(buffpos, ' ');
    buffpos = copyStdioFloatFormat(buffpos, spec, prec);
    buffpos = copyChar(buffpos, ' ');
    buffpos = copyStdioFloatFormat(buffpos, spec, prec);
    *buffpos = '\0';
    return txtformat;
}

inline char* copyCoords(
        char* buffer,
        const TextFormatVec3f& format,
        const Vec3f& coords)
{
#if GMIO_FLOAT2STR_LIB == GMIO_FLOAT2STR_LIB_DOUBLE_CONVERSION
    const FloatTextFormat coord_format = format.coord_format;
    const uint8_t coord_prec = format.coord_prec;
    buffer += GDC_float2str(coords.x, buffer, 32, coord_format, coord_prec);
    buffer = copyChar(buffer, ' ');
    buffer += GDC_float2str(coords.y, buffer, 32, coord_format, coord_prec);
    buffer = copyChar(buffer, ' ');
    buffer += GDC_float2str(coords.z, buffer, 32, coord_format, coord_prec);
    return buffer;
#else
    return buffer + std::sprintf(buffer,
                                 format.str_printf_format.data(),
                                 coords.x, coords.y, coords.z);
#endif
}

} // namespace

int STL_writeAscii(
        FuncWriteData func_write,
        const STL_Mesh& mesh,
        STL_WriteOptions options)
{
    Task* task = options.task;
    const IoBufferHelper buffer_helper(options.buffer);
    Span<uint8_t> buffer = buffer_helper.get();
    const uint32_t buffer_facet_count = buffer.size() / STL_AsciiFacetSizeP2;
    auto const buffstart = reinterpret_cast<char*>(buffer.data());

    // Initialize TextFormatVec3f
    const uint8_t float32_prec =
            options.ascii_float32_prec != 0 ? options.ascii_float32_prec : 9;
    const TextFormatVec3f vec_txtformat = {
        options.ascii_float32_format,
        float32_prec,
        coordsPrintfFormat(float32_prec, options.ascii_float32_format)
    };

    // Check validity of input parameters
    if (!options.ascii_dont_check_lc_numeric && !lc_numeric_is_C())
        return Error_BadLcNumeric;
    if (!STL_asciiCheckFloatPrecision(vec_txtformat.coord_prec))
        return STL_Error_InvalidFloat32Prec;

    // Write solid declaration
    if (!options.write_triangles_only) {
        char* buffpos = buffstart;
        buffpos = copyString(buffpos, "solid ");
        buffpos = copyString(buffpos, options.ascii_solid_name);
        buffpos = copyEol(buffpos);
        if (!writeExactly(func_write, buffstart, buffpos - buffstart))
            return Error_Stream;
    }

    // Write solid's facets
    for (uint32_t i = 0; i < mesh.triangleCount(); i += buffer_facet_count) {
        if (task != nullptr)
            task->handleProgress(i, mesh.triangleCount());

        const uint32_t clamped_facet_count =
                std::min(i + buffer_facet_count, mesh.triangleCount());
        char* buffpos = buffstart;
        // Writing of facets is buffered
        for (uint32_t ibuffer_facet = i;
             ibuffer_facet < clamped_facet_count;
             ++ibuffer_facet)
        {
            const STL_Triangle tri = mesh.triangle(ibuffer_facet);

            buffpos = copyString(buffpos, "facet normal ");
            buffpos = copyCoords(buffpos, vec_txtformat, tri.n);

            buffpos = copyString(buffpos, "\nouter loop");
            buffpos = copyString(buffpos, "\n vertex ");
            buffpos = copyCoords(buffpos, vec_txtformat, tri.v1);
            buffpos = copyString(buffpos, "\n vertex ");
            buffpos = copyCoords(buffpos, vec_txtformat, tri.v2);
            buffpos = copyString(buffpos, "\n vertex ");
            buffpos = copyCoords(buffpos, vec_txtformat, tri.v3);
            buffpos = copyString(buffpos, "\nendloop");

            buffpos = copyString(buffpos, "\nendfacet\n");
        } // end for (ibuffer_facet)

        if (!writeExactly(func_write, buffstart, buffpos - buffstart))
            return Error_Stream;
        if (task != nullptr && task->isStopRequested())
            return Error_TaskStopped;
    } // end for (i)

    // Write end of solid
    if (!options.write_triangles_only) {
        char* buffpos = buffstart;
        buffpos = copyString(buffpos, "endsolid ");
        buffpos = copyString(buffpos, options.ascii_solid_name);
        buffpos = copyEol(buffpos);
        if (!writeExactly(func_write, buffstart, buffpos - buffstart))
            return Error_Stream;
    }

    return Error_OK;
}

} // namespace gmio
