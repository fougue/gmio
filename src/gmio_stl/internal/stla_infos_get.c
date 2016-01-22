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
        struct gmio_stl_infos_get_args* args, unsigned flags)
{
    const gmio_bool_t flag_facet_count =
            (flags & GMIO_STL_INFO_FLAG_FACET_COUNT) != 0;
    const gmio_bool_t flag_size =
            (flags & GMIO_STL_INFO_FLAG_SIZE) != 0;
    const gmio_bool_t flag_stla_solidname =
            (flags & GMIO_STLA_INFO_FLAG_SOLIDNAME) != 0;

    struct gmio_stl_infos* infos = &args->infos;
    void* const mblock_ptr = args->stream_memblock.ptr;
    /* Leave one byte to end the string buffer with 0 */
    const size_t mblock_size = args->stream_memblock.size - 1;
    struct gmio_stla_parse_data parse_data = {0};
    struct gmio_stringstream* sstream = &parse_data.strstream;
    struct gmio_string* strbuff = &parse_data.strbuff;
    char fixed_buffer[GMIO_STLA_READ_STRING_MAX_LEN];

    int err = GMIO_ERROR_OK;

    if (flags == 0)
        return err;
    if (!gmio_check_memblock(&err, &args->stream_memblock))
        return err;

    parse_data.strstream =
            gmio_stringstream(
                args->stream,
                gmio_string(mblock_ptr, 0, mblock_size));
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

#if 0
#include "../../gmio_core/error.h"
#include "../../gmio_core/internal/min_max.h"
#include "../../gmio_core/internal/string.h"
#include "../../gmio_core/internal/string_ascii_utils.h"
#include "stl_rw_common.h"

#include <string.h>

static uint32_t stla_facet_count(
        const struct gmio_string* strbuff, const char* end_ptr)
{
    const char* substr_at = NULL;
    size_t strbuff_pos = 0;
    uint32_t facet_count = 0;

    do {
        substr_at = gmio_ascii_istrstr(strbuff->ptr + strbuff_pos, "endfacet");
        if (substr_at != NULL && substr_at < end_ptr) {
            ++facet_count;
            /* Note: strlen("endfacet") == 8 */
            strbuff_pos = (substr_at - strbuff->ptr) + 8;
        }
        else {
            substr_at = NULL;
        }
    } while (substr_at != NULL);

    return facet_count;
}

enum {
    BUFF_OVERLAP_SIZE = 14,
    BUFF_OVERLAP_SIZE_DIV2 = BUFF_OVERLAP_SIZE / 2
};

int gmio_stla_infos_get(
        struct gmio_stl_infos_get_args* args, unsigned flags)
{
    struct gmio_stream* stream = &args->stream;
    struct gmio_stl_infos* infos = &args->infos;
    void* mblock_ptr = args->stream_memblock.ptr;
    /* Leave one byte to end the string buffer with 0 */
    const size_t mblock_size = args->stream_memblock.size - 1;
    struct gmio_string strbuff = gmio_string(mblock_ptr, 0, mblock_size);

    const gmio_bool_t flag_facet_count =
            (flags & GMIO_STL_INFO_FLAG_FACET_COUNT) != 0;
    const gmio_bool_t flag_size =
            (flags & GMIO_STL_INFO_FLAG_SIZE) != 0;
    const gmio_bool_t flag_stla_solidname =
            (flags & GMIO_STLA_INFO_FLAG_SOLIDNAME) != 0;

    int err = GMIO_ERROR_OK;

    if (!gmio_check_memblock(&err, &args->stream_memblock))
        return err;

    if (flags != 0) {
        /* 'overlap' stores the ending/starting bytes of the previous/current
         * stream buffers(memblock) */
        char overlap[14] = {0}; /* 14 == 2*(strlen("endfacet") - 1) */
        gmio_bool_t endsolid_found = GMIO_FALSE;

        while (!endsolid_found && gmio_no_error(err)) {
            const char* substr_at = NULL;
            const size_t read_size =
                    gmio_stream_read(stream, mblock_ptr, 1, mblock_size);
            const int stream_err = gmio_stream_error(&args->stream);
            const gmio_bool_t overlap_has_contents = overlap[0] != 0;
            gmio_bool_t endsolid_in_overlap = GMIO_FALSE;

            err = stream_err;
            strbuff.len = read_size;
            strbuff.ptr[strbuff.len] = 0;

            /* Copy first half of overlap buffer */
            if (overlap_has_contents) {
                strncpy(&overlap[BUFF_OVERLAP_SIZE_DIV2],
                        mblock_ptr,
                        GMIO_MIN(BUFF_OVERLAP_SIZE_DIV2, read_size));
            }

            /* Find "endsolid" in overlap */
            if (overlap_has_contents) {
                substr_at = strstr(overlap, "endsolid");
                endsolid_found = substr_at != NULL;
                endsolid_in_overlap = endsolid_found;
            }

            /* Find "endsolid" in memblock */
            if (!endsolid_found) {
                substr_at = gmio_ascii_istrstr(strbuff.ptr, "endsolid");
                endsolid_found = substr_at != NULL;
            }

            /* Update stream size */
            if (flag_size) {
                /* Note: strlen("endsolid") == 8 */
                if (endsolid_found) {
                    if (!endsolid_in_overlap)
                        infos->size += (substr_at - strbuff.ptr) + 8;
                    /* TODO : gérer le cas où "endsolid" se trouve dans overlap */
                }
                else {
                    infos->size += read_size;
                }
            }

            /* Find "endfacet" tokens */
            if (flag_facet_count && !endsolid_in_overlap) {
                const char* endsolid_ptr =
                        endsolid_found ? substr_at : gmio_string_end(&strbuff);
                /* Check in overlap */
                const gmio_bool_t endfacet_in_overlap =
                        overlap_has_contents
                        && strstr(overlap, "endfacet") != NULL;
                infos->facet_count += endfacet_in_overlap ? 1 : 0;
                /* Check in memblock */
                infos->facet_count += stla_facet_count(&strbuff, endsolid_ptr);
            }

            /* Copy second half of overlap buffer */
            if (!endsolid_found && read_size >= BUFF_OVERLAP_SIZE_DIV2) {
                memset(&overlap, 0, sizeof(overlap));
                strncpy(overlap,
                        &strbuff.ptr[read_size - BUFF_OVERLAP_SIZE_DIV2],
                        GMIO_MIN(BUFF_OVERLAP_SIZE_DIV2, read_size));
            }
        }
    }

    return err;
}
#endif
