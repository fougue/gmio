/****************************************************************************
** gmio
** Copyright Fougue (2 Mar. 2015)
** contact@fougue.pro
**
** This software is a reusable library whose purpose is to provide complete
** I/O support for various CAD file formats (eg. STL)
**
** This software is governed by the CeCILL-B license under French law and
** abiding by the rules of distribution of free software.  You can  use,
** modify and/ or redistribute the software under the terms of the CeCILL-B
** license as circulated by CEA, CNRS and INRIA at the following URL
** "http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html".
****************************************************************************/

#include "stla_infos_get.h"

#include "../../gmio_core/error.h"
#include "../../gmio_core/internal/helper_memblock.h"
#include "../../gmio_core/internal/min_max.h"
#include "../../gmio_core/internal/stringstream.h"
#include "../stl_error.h"
#include "stla_parsing.h"
#include "stl_rw_common.h"

#include <string.h>

static const char* gmio_stringstream_skip_until_ascii_space(
        struct gmio_stringstream* sstream)
{
    const char* curr_char = gmio_stringstream_current_char(sstream);
    while (curr_char != NULL && !gmio_ascii_isspace(*curr_char))
        curr_char = gmio_stringstream_next_char(sstream);
    return curr_char;
}

static const char* gmio_stringstream_skip_word(
        struct gmio_stringstream* sstream)
{
    gmio_stringstream_skip_ascii_spaces(sstream);
    return gmio_stringstream_skip_until_ascii_space(sstream);
}

static void gmio_skip_xyz_coords(struct gmio_stla_parse_data* data)
{
    struct gmio_stringstream* sstream = &data->strstream;
    gmio_stringstream_skip_word(sstream);
    gmio_stringstream_skip_word(sstream);
    gmio_stringstream_skip_word(sstream);
    data->strbuff.len = 0;
    data->token = unknown_token;
}

static int gmio_skip_facet(struct gmio_stla_parse_data* data)
{
    int errc = 0;
    if (data->token != FACET_token)
        errc += gmio_stla_eat_next_token_inplace(data, FACET_token);

    errc += gmio_stringstream_skip_word(&data->strstream) == NULL; /* normal */
    gmio_skip_xyz_coords(data);

    errc += gmio_stringstream_skip_word(&data->strstream) == NULL; /* outer */
    errc += gmio_stringstream_skip_word(&data->strstream) == NULL; /* loop */

    errc += gmio_stringstream_skip_word(&data->strstream) == NULL; /* vertex */
    gmio_skip_xyz_coords(data);
    errc += gmio_stringstream_skip_word(&data->strstream) == NULL; /* vertex */
    gmio_skip_xyz_coords(data);
    errc += gmio_stringstream_skip_word(&data->strstream) == NULL; /* vertex */
    gmio_skip_xyz_coords(data);

    errc += gmio_stringstream_skip_word(&data->strstream) == NULL; /* endloop */
    errc += gmio_stringstream_skip_word(&data->strstream) == NULL; /* endfacet */

    return errc;
}

static int gmio_stla_check_next_token(
        struct gmio_stringstream* sstream, const char* expected_token_str)
{
    const char* stream_char = NULL;

    stream_char = gmio_stringstream_skip_ascii_spaces(sstream);
    for (;;) {
        if (stream_char == NULL || gmio_ascii_isspace(*stream_char)) {
            if (*expected_token_str == 0)
                return 1;
            break;
        }
        else if (!gmio_ascii_char_iequals(*stream_char, *expected_token_str)
                 || *expected_token_str == 0)
        {
            break;
        }
        stream_char = gmio_stringstream_next_char(sstream);
        ++expected_token_str;
    }
    gmio_stringstream_skip_word(sstream);
    return 0;
}

static void gmio_stringstream_stla_read_hook(
        void* cookie, const struct gmio_string* strbuff)
{
    struct gmio_stringstream_stla_cookie* tcookie =
            (struct gmio_stringstream_stla_cookie*)(cookie);
    if (tcookie != NULL)
        tcookie->stream_offset += strbuff->len;
}

int gmio_stla_infos_get(
        struct gmio_stl_infos* infos,
        struct gmio_stream stream,
        unsigned flags,
        const struct gmio_stl_infos_get_options* opts)
{
    const bool flag_facet_count =
            (flags & GMIO_STL_INFO_FLAG_FACET_COUNT) != 0;
    const bool flag_size =
            (flags & GMIO_STL_INFO_FLAG_SIZE) != 0;
    const bool flag_stla_solidname =
            (flags & GMIO_STLA_INFO_FLAG_SOLIDNAME) != 0;

    void* const mblock_ptr = opts->stream_memblock.ptr;
    /* Leave one byte to end the string buffer with 0 */
    const size_t mblock_size = opts->stream_memblock.size - 1;
    struct gmio_stla_parse_data parse_data = {0};
    struct gmio_stringstream* sstream = &parse_data.strstream;
    struct gmio_string* strbuff = &parse_data.strbuff;
    char fixed_buffer[GMIO_STLA_READ_STRING_MAX_LEN];

    int err = GMIO_ERROR_OK;

    if (flags == 0)
        return err;
    if (!gmio_check_memblock(&err, &opts->stream_memblock))
        return err;

    parse_data.strstream =
            gmio_stringstream(stream, gmio_string(mblock_ptr, 0, mblock_size));
    parse_data.strstream.func_stream_read_hook = gmio_stringstream_stla_read_hook;
    parse_data.strbuff = gmio_string(fixed_buffer, 0, sizeof(fixed_buffer));

    gmio_stringstream_skip_word(sstream); /* Skip "\s*solid" */
    gmio_stla_parse_solidname_beg(&parse_data);
    if (flag_stla_solidname) {
        const size_t name_len_for_cpy =
                GMIO_MIN(infos->stla_solidname_maxlen, strbuff->len);
        strncpy(infos->stla_solidname, strbuff->ptr, name_len_for_cpy);
        /* Null terminate C string */
        if (name_len_for_cpy != 0)
            infos->stla_solidname[name_len_for_cpy - 1] = 0;
        else if (infos->stla_solidname_maxlen != 0)
            infos->stla_solidname[0] = 0;
        gmio_string_clear(strbuff);
    }

    if (flag_facet_count) {
        infos->facet_count = 0;
        while (parse_data.token == FACET_token
               && !parse_data.error
               && gmio_stringstream_current_char(sstream) != NULL)
        {
            if (gmio_skip_facet(&parse_data) == 0) {
                ++infos->facet_count;
                /* Eat next unknown token */
                strbuff->len = 0;
                gmio_stringstream_eat_word(sstream, strbuff);
                parse_data.token =
                        gmio_stla_find_token(strbuff->ptr, strbuff->len);
            }
        }
    }

    if (flag_size) {
        if (flag_facet_count) { /* We should be near "endsolid" token */
            if (parse_data.token != ENDSOLID_token)
                gmio_stla_eat_next_token(&parse_data, ENDSOLID_token);
        }
        else {
            while (!gmio_stla_check_next_token(sstream, "endsolid")
                   && !parse_data.error
                   && gmio_stringstream_current_char(sstream) != NULL)
            {
            }
        }
        infos->size = parse_data.strstream_cookie.stream_offset;
        infos->size += sstream->strbuff_at - strbuff->ptr;
    }

    return err;
}
