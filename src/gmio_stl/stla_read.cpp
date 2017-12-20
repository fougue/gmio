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

#include "../gmio_core/error.h"
#include "../gmio_core/internal/buffer_read_helper.h"
#include "../gmio_core/internal/io_buffer_helper.h"
#include "../gmio_core/internal/locale_utils.h"
#include "../gmio_core/internal/scoped_resources.h"
#include "../gmio_core/internal/text_istream.h"
#include "../gmio_stl/stl_error.h"
#include "../gmio_stl/internal/stl_ascii_parser.h"
#include "../gmio_stl/internal/stl_ascii_parser.h"

namespace gmio {

int STL_readAscii(
        FuncReadData func_read,
        STL_MeshCreator* creator,
        STL_ReadOptions options)
{
    // Check validity of input parameters
    if (!options.ascii_dont_check_lc_numeric && !lc_numeric_is_C())
        return Error_BadLcNumeric;

    const IoBufferHelper buffer_helper(options.buffer);
    Span<uint8_t> buffer = buffer_helper.get();
    options.buffer = buffer;

    ScopedResources scoped_res;
    if (options.buffer_initial_contents_size > 0) {
        auto initial_data = makeSpan(buffer.data(), options.buffer_initial_contents_size);
        auto helper = scoped_res.emplace<BufferReadHelper>(func_read, initial_data);
        func_read = BufferReadHelper_funcReadData(helper);
    }

    bool is_stop_requested = false;
    if (options.task != nullptr) {
        auto istreampos = scoped_res.emplace<uint64_t>(0);
        FuncReadData base_func_read = std::move(func_read);
        func_read = [=, &is_stop_requested](uint8_t* dst, uint64_t size, ReadState* state) {
            const uint64_t len_read = base_func_read(dst, size, state);
            *istreampos += len_read;
            is_stop_requested = options.task->isStopRequested();
            options.task->handleProgress(*istreampos, options.ascii_solid_size);
            return len_read;
        };
    }

    TextIStream istream(buffer, func_read);
    STL_AsciiParser parser(&istream);

    // Begin solid
    STL_MeshCreatorInfos infos = {};
    infos.ascii_solid_size = options.ascii_solid_size;
    infos.format = STL_Format_Ascii;
    if (!parser.parseBeginSolid(&infos.ascii_solid_name))
        return STL_Error_Parsing;
    creator->beginSolid(infos);
    // Parse facets
    STL_Triangle triangle = {};
    for (uint32_t ifacet = 0;
         parser.currentToken() == STL_AsciiParser::Token_Facet;
         ++ifacet)
    {
        if (!parser.parseFacet(&triangle))
            return STL_Error_Parsing;
        creator->addTriangle(ifacet, triangle);
        if (is_stop_requested)
            return Error_TaskStopped;
    }
    // End solid
    if (!parser.parseEndSolid())
        return STL_Error_Parsing;
    creator->endSolid();
    return Error_OK;
}

} // namespace gmio
