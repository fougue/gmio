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

#include "stla_infos_probe.h"

#include "../../gmio_core/internal/io_buffer_helper.h"
#include "../../gmio_core/internal/text_istream.h"
#include "stl_ascii_parser.h"

#include <functional>

namespace gmio {

namespace {

constexpr bool ascii_isNotSpace(char c) {
    return !ascii_isSpace(c);
}

constexpr bool ascii_isNotEndOfLine(char c) {
    return c != '\n' && c != '\r';
}

} // namespace

Result<STL_Infos> STL_probeAsciiInfos(
        FuncReadData func_read,
        unsigned flags,
        STL_ProbeInfosOptions options)
{
    const bool flag_facet_count =
            (flags & STL_InfoFlag_FacetCount) != 0;
    const bool flag_size =
            (flags & STL_InfoFlag_Size) != 0;
    const bool flag_ascii_solidname =
            (flags & STL_InfoFlag_AsciiSolidName) != 0;

    STL_Infos infos = {};
    const IoBufferHelper buff_helper(options.buffer);
    Span<uint8_t> buffer = buff_helper.get();

    if (flags == 0)
        return infos;
    if ((flags & STL_InfoFlag_Format) != 0)
        infos.format = STL_Format_Ascii;

    if (flag_size) {
        func_read = [=, &infos](uint8_t* dst, uint64_t size, ReadState* state) {
            const uint64_t readsize = func_read(dst, size, state);
            infos.size += readsize;
            return readsize;
        };
    }
    TextIStream text_stream(buffer, func_read);
    if (flag_ascii_solidname) {
        STL_AsciiParser parser(&text_stream);
        parser.parseBeginSolid(&infos.ascii_solid_name);
        if (flag_facet_count
                && parser.currentToken() == STL_AsciiParser::Token_Facet)
        {
            ++infos.facet_count;
        }
    }

    if (flag_facet_count) {
        bool endfound = false;
        while (!endfound) {
            const char* c = text_stream.skipAsciiSpaces();
            if (c != nullptr) {
                if (ascii_iequals(*c, 'f')) {
                    text_stream.nextChar();
                    if (text_stream.eatExactly("acet", Case_Insensitive))
                        ++infos.facet_count;
                }
                else if (ascii_iequals(*c, 'e')) {
                    text_stream.nextChar();
                    endfound = text_stream.eatExactly("ndsolid", Case_Insensitive);
                }
            }
            else {
                endfound = true;
            }
            text_stream.skipWhile(ascii_isNotSpace);
        }
    }

    if (flag_size) {
        // On case flag_facet_count is on, we should already be after "endsolid"
        // token
        if (!flag_facet_count) {
            bool endfound = false;
            while (!endfound) {
                const char* c = text_stream.skipAsciiSpaces();
                if (c != nullptr) {
                    if (ascii_iequals(*c, 'e')) {
                        text_stream.nextChar();
                        endfound = text_stream.eatExactly("ndsolid", Case_Insensitive);
                    }
                }
                else {
                    endfound = true;
                }
                text_stream.skipWhile(ascii_isNotSpace);
            }
        }
        // Eat whole line containing "endsolid"
        text_stream.skipWhile(ascii_isNotEndOfLine);
        // Try to move stream pos on EOF or next solid
        text_stream.skipAsciiSpaces();

        // Whitespaces were just previously skipped, the stream pos can be
        // at either :
        //    - on EOF => fine, stream is exhausted
        //    - on some non-W/S => stream is then one char too far
        // These two cases are detected by testing result of the
        // stringstream's current char
        const int end_offset =
                text_stream.currentChar() != nullptr ? 1 : 0;
        infos.size -= text_stream.bufferEnd() - text_stream.currentChar() + end_offset;
    }

    return infos;
}

} // namespace gmio
