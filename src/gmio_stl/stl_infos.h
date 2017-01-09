/****************************************************************************
** Copyright (c) 2016, Fougue Ltd. <http://www.fougue.pro>
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

/*! \file stl_infos.h
 *  Retrieval of STL infos from input stream
 *
 *  \addtogroup gmio_stl
 *  @{
 */

#ifndef GMIO_STL_INFOS_H
#define GMIO_STL_INFOS_H

#include "stl_global.h"
#include "stl_format.h"
#include "stlb_header.h"
#include <stddef.h>

/*! Informations retrieved by gmio_stl_infos_probe() */
struct gmio_stl_infos
{
    /*! STL format of the input stream */
    enum gmio_stl_format format;

    /*! Count of facets(triangles) */
    uint32_t facet_count;

    /*! Size of the STL data in bytes.
     *  For STL ascii it includes the "endsolid" tag */
    gmio_streamsize_t size;

    /*! STL ascii only: name of the solid.
     *  The pointer has to be set before calling gmio_stl_infos_probe()
     *  \sa stla_solidname_maxlen */
    char* stla_solidname;

    /*! STL ascii only: maximum length(capacity) of stla_solidname.
     *  The value has to be set before calling gmio_stl_infos_probe()
     *  \sa stla_solidname */
    size_t stla_solidname_maxlen;

    /*! STL binary only: header(80-bytes) of STL data */
    struct gmio_stlb_header stlb_header;
};

/*! Flags(OR-combinations) for each STL info */
enum gmio_stl_info_flag
{
    /*! -> gmio_stl_infos::facet_count */
    GMIO_STL_INFO_FLAG_FACET_COUNT = 0x0001,

    /*! -> gmio_stl_infos::size */
    GMIO_STL_INFO_FLAG_SIZE = 0x0002,

    /*! -> gmio_stl_infos::stla_solidname */
    GMIO_STLA_INFO_FLAG_SOLIDNAME = 0x0004,

    /*! -> gmio_stl_infos::stlb_header */
    GMIO_STLB_INFO_FLAG_HEADER = 0x0008,

    /*! -> gmio_stl_infos::stla_solidname or gmio_stl_infos::stlb_header */
    GMIO_STL_INFO_FLAG_SOLIDNAME_OR_HEADER =
        GMIO_STLA_INFO_FLAG_SOLIDNAME | GMIO_STLB_INFO_FLAG_HEADER,

    /*! -> gmio_stl_infos::format */
    GMIO_STL_INFO_FLAG_FORMAT = 0x0010,

    /*! All infos */
    GMIO_STL_INFO_FLAG_ALL = 0xFFFF
};

/*! Options of function gmio_stl_infos_probe() */
struct gmio_stl_infos_probe_options
{
    /*! See gmio_stl_read_options::stream_memblock */
    struct gmio_memblock stream_memblock;

    /*! Assume STL input format, if GMIO_STL_FORMAT_UNKNOWN then it is
     *  automatically guessed */
    enum gmio_stl_format format_hint;

    /*! Restrict gmio_stl_infos_probe() to not read further this limit(in bytes)
     *  \warning Not yet supported */
    gmio_streamsize_t size_limit;
};

GMIO_C_LINKAGE_BEGIN

/*! Finds informations about STL contents
 *
 *  \p infos is an output parameter that will hold the retrieved informations
 *
 *  \p flags is a bitor combination of \c gmio_stl_info_flag values and is used
 *  to select the informations to retrieve.
 *
 *  \pre <tt> infos != NULL </tt>
 *  \pre <tt> stream != NULL </tt>
 *
 *  \p options can be safely set to \c NULL in this case default values are used
 *
 *  \return Error code (see gmio_core/error.h and stl_error.h)
 */
GMIO_API int gmio_stl_infos_probe(
                struct gmio_stl_infos* infos,
                struct gmio_stream* stream,
                unsigned flags,
                const struct gmio_stl_infos_probe_options* options);

/*! Finds informations about STL contents from a file
 *
 *  This is just a facility function over gmio_stl_infos_probe(). The internal
 *  object is created to read file at \p filepath.
 *
 *  \pre <tt> infos != NULL </tt>
 *  \pre <tt> filepath != \c NULL </tt>\n
 *       The file is opened with \c fopen() so \p filepath shall follow the file
 *       name specifications of the running environment
 *
 *  \sa gmio_stl_infos_probe(), gmio_stream_stdio(FILE*)
 */
GMIO_API int gmio_stl_infos_probe_file(
                struct gmio_stl_infos* infos,
                const char* filepath,
                unsigned flags,
                const struct gmio_stl_infos_probe_options* options);

/*! Returns the size(in bytes) of the next STL ascii solid in \p stream
 *
 *  It is a facade over gmio_stl_infos_probe() for gmio_stl_infos::size only
 *
 *  Pointer to this function can be given to
 *  gmio_stl_read_options::func_stla_get_streamsize() and is useful when
 *  reading in sequence multi solids in STL ascii. The stream can be cleanly
 *  advanced solid by solid after each call to gmio_stl_read()
 *
 *  \pre <tt> stream != NULL </tt>
 *  \pre <tt> stream_memblock != NULL </tt>
 */
GMIO_API gmio_streamsize_t gmio_stla_infos_probe_streamsize(
                struct gmio_stream* stream,
                struct gmio_memblock* stream_memblock);

GMIO_C_LINKAGE_END

#endif /* GMIO_STL_INFOS_H */
/*! @} */
