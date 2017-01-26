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

#include "../../gmio_core/error.h"
#include "../../gmio_core/internal/error_check.h"
#include "../../gmio_core/internal/helper_stream.h"
#include "../../gmio_core/internal/helper_memblock.h"
#include "../../gmio_core/internal/min_max.h"
#include "../../gmio_core/internal/stringstream.h"
#include "../stl_error.h"
#include "stla_parsing.h"
#include "stl_error_check.h"

#include <string.h>

/* Skip characters until an ASCII space is met */
static const char* gmio_stringstream_skip_until_ascii_space(
        struct gmio_stringstream* sstream)
{
    const char* curr_char = gmio_stringstream_current_char(sstream);
    while (curr_char != NULL && !gmio_ascii_isspace(*curr_char))
        curr_char = gmio_stringstream_next_char(sstream);
    return curr_char;
}

/* Eats \p str case-insensitive, stops on any difference */
static bool gmio_stringstream_icase_eat(
        struct gmio_stringstream* sstream, const char* str)
{
    const char* c = gmio_stringstream_next_char(sstream);
    while (c != NULL && *str != 0) {
        if (!gmio_ascii_char_iequals(*c, *str))
            return false;
        c = gmio_stringstream_next_char(sstream);
        ++str;
    }
    return *str == 0;
}

/* Callback invoked by gmio_stringstream for handling also stream total size */
static size_t gmio_stringstream_read__flagsize(
        void* cookie, struct gmio_stream* stream, char* ptr, size_t len)
{
    gmio_streamsize_t* ptr_size = (gmio_streamsize_t*)(cookie);
    const size_t len_read = gmio_stream_read_bytes(stream, ptr, len);
    *ptr_size += len_read;
    return len_read;
}

int gmio_stla_infos_probe(
        struct gmio_stl_infos* infos,
        struct gmio_stream* stream,
        unsigned flags,
        const struct gmio_stl_infos_probe_options* opts)
{
    const bool flag_facet_count =
            (flags & GMIO_STL_INFO_FLAG_FACET_COUNT) != 0;
    const bool flag_size =
            (flags & GMIO_STL_INFO_FLAG_SIZE) != 0;
    const bool flag_stla_solidname =
            (flags & GMIO_STLA_INFO_FLAG_SOLIDNAME) != 0;

    void* const mblock_ptr = opts->stream_memblock.ptr;
    /* Leave one byte to end of string buffer */
    const size_t mblock_size = opts->stream_memblock.size - 1;
    struct gmio_stringstream sstream = {0};
    int err = GMIO_ERROR_OK;

    if (flags == 0)
        return err;
    if (!gmio_check_memblock(&err, &opts->stream_memblock))
        return err;

    /* Initialize string stream */
    sstream.stream = *stream;
    sstream.strbuff = gmio_string(mblock_ptr, 0, mblock_size);
    if (flag_size) {
        infos->size = 0;
        sstream.cookie = &infos->size;
        sstream.func_stream_read = gmio_stringstream_read__flagsize;
    }
    else {
        sstream.func_stream_read = gmio_stringstream_default_func_read;
    }
    gmio_stringstream_init_pos(&sstream);

    if (flag_stla_solidname) {
        struct gmio_stla_parse_data parse_data = {0};
        char fixed_buffer[GMIO_STLA_READ_STRING_MAX_LEN] = {0};

        parse_data.strstream = sstream;
        parse_data.token_str = gmio_string(fixed_buffer, 0, sizeof(fixed_buffer));

        /* Skip "\s*solid" */
        gmio_stringstream_skip_ascii_spaces(&parse_data.strstream);
        gmio_stringstream_skip_until_ascii_space(&parse_data.strstream);

        /* Eat solid name */
        gmio_stla_parse_solidname_beg(&parse_data);

        /* Copy parsed solid name into infos->stla_solid_name */
        {
            const struct gmio_string* strbuff = &parse_data.token_str;
            const size_t name_len_for_cpy =
                    GMIO_MIN(infos->stla_solidname_maxlen - 1, strbuff->len);

            strncpy(infos->stla_solidname, strbuff->ptr, name_len_for_cpy);
            /* Null terminate C string */
            if (name_len_for_cpy != 0)
                infos->stla_solidname[name_len_for_cpy] = 0;
            else if (infos->stla_solidname_maxlen != 0)
                infos->stla_solidname[0] = 0;
        }
        sstream = parse_data.strstream;
    }

    if (flag_facet_count) {
        bool endfound = false;

        infos->facet_count = 0;

        while (!endfound) {
            const char* c = gmio_stringstream_skip_ascii_spaces(&sstream);
            if (c != NULL) {
                if (gmio_ascii_char_iequals(*c, 'f')) {
                    if (gmio_stringstream_icase_eat(&sstream, "acet"))
                        ++infos->facet_count;
                }
                else {
                    endfound =
                            gmio_ascii_char_iequals(*c, 'e')
                            && gmio_stringstream_icase_eat(&sstream, "ndsolid");
                }
            }
            else {
                endfound = true;
            }
            gmio_stringstream_skip_until_ascii_space(&sstream);
        }
    }

    if (flag_size) {
        /* On case flag_facet_count is on, we should already be after "endsolid"
         * token */
        if (!flag_facet_count) {
            bool endfound = false;
            while (!endfound) {
                const char* c = gmio_stringstream_skip_ascii_spaces(&sstream);
                if (c != NULL) {
                    endfound =
                            gmio_ascii_char_iequals(*c, 'e')
                            && gmio_stringstream_icase_eat(&sstream, "ndsolid");
                }
                else {
                    endfound = true;
                }
                gmio_stringstream_skip_until_ascii_space(&sstream);
            }
        }
        {
            /* Eat whole line containing "endsolid" */
            const char* c = gmio_stringstream_current_char(&sstream);
            while (c != NULL && *c != '\n' && *c != '\r')
                c = gmio_stringstream_next_char(&sstream);
            /* Try to move stream pos on EOF or next solid */
            gmio_stringstream_skip_ascii_spaces(&sstream);
        }
        {
            /* Whitespaces were just previously skipped, the stream pos can at
             * either :
             *     - on EOF => fine, stream is exhausted
             *     - on some non-W/S => stream is then one char too far
             * These two cases are detected by testing result of the
             * stringstream's current char */
            const int end_offset =
                    gmio_stringstream_current_char(&sstream) != NULL ? 1 : 0;
            infos->size -= sstream.strbuff_end - sstream.strbuff_at + end_offset;
            infos->size = GMIO_MAX(0, infos->size);
        }
    }

    return err;
}
