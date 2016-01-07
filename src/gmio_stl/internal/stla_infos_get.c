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
#include "../../gmio_core/internal/string.h"
#include "../../gmio_core/internal/string_utils.h"
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
