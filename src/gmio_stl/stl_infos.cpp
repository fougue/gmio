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

#include "stl_infos.h"

#include "../gmio_core/error.h"
#include "../gmio_core/internal/buffer_read_helper.h"
#include "../gmio_core/internal/scoped_resources.h"
#include "stl_error.h"
#include "stl_format.h"
#include "internal/stla_infos_probe.h"
#include "internal/stlb_infos_probe.h"

namespace gmio {

Result<STL_Infos> STL_probeInfos(
        FuncReadData func_read,
        unsigned flags,
        STL_ProbeInfosOptions options)
{
    ScopedResources scoped_res;

    // Guess format when left unspecified
    if (options.format_hint == STL_Format_Unknown) {
        uint8_t buff[512];
        const uint64_t read_size = func_read(buff, sizeof(buff), nullptr);
        options.format_hint = STL_probeFormat(makeSpan(buff, read_size));
        if (options.format_hint == STL_Format_Unknown)
            return makeError(STL_Error_UnknownFormat);
        auto helper = scoped_res.emplace<BufferReadHelper>(
                    std::move(func_read), makeSpan(buff, read_size));
        func_read = BufferReadHelper_funcReadData(helper);
    }

    // Dispatch to the sub-function
    if (options.format_hint == STL_Format_Ascii)
        return STL_probeAsciiInfos(func_read, flags, options);
    else if ((options.format_hint & STL_Format_TagBinary) != 0)
        return STL_probeBinaryInfos(func_read, flags, options);
    else
        return makeError(STL_Error_UnknownFormat);
}

Result<STL_Infos> STL_probeInfos(
        const char* filepath,
        unsigned flags,
        STL_ProbeInfosOptions options)
{
    FILE* file = std::fopen(filepath, "rb");
    if (file != nullptr) {
        const Result<STL_Infos> result =
                STL_probeInfos(FILE_funcReadData(file), flags, options);
        std::fclose(file);
        return result;
    }
    return makeError(Error_Stdio);
}

uint64_t STL_probeAsciiSolidSize(FuncReadData func_read)
{
    STL_ProbeInfosOptions options = {};
    options.format_hint = STL_Format_Ascii;
    const Result<STL_Infos> infos =
            STL_probeInfos(std::move(func_read), STL_InfoFlag_Size, options);
    return infos ? infos.data().size : 0;
}

} // namespace gmio
