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

/*! \file stl_infos.h
 *  Retrieval of STL infos from input stream
 *
 *  \addtogroup gmio_stl
 *  @{
 */

#ifndef GMIO_STL_INFOS_H
#define GMIO_STL_INFOS_H

#include "stl_global.h"

#include "../gmio_core/internal/helper_stream.h"

#include <stddef.h>

#include "stl_format.h"
#include "stlb_header.h"

/*! Informations retrieved by gmio_stl_infos_get() */
struct gmio_stl_infos
{
    /*! STL format of the input stream */
    enum gmio_stl_format format;

    /*! Count of facets(triangles) */
    uint32_t facet_count;

    /*! Size of the STL data in bytes
     *
     *  For STL ascii it includes the "endsolid" tag */
    gmio_streamsize_t size;

    /*! STL ascii only: name of the solid
     *
     *  The pointer has to be set before calling gmio_stl_infos_get() */
    char* stla_solidname;

    /*! STL ascii only: maximum length(capacity) of stla_solidname
     *
     *  The value has to be set before calling gmio_stl_infos_get()
     */
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

/*! Options of function gmio_stl_infos_get() */
struct gmio_stl_infos_get_options
{
    /*! See gmio_core_readwrite_options::stream_memblock */
    struct gmio_memblock stream_memblock;

    /*! Assume STL input format, if GMIO_STL_FORMAT_UNKNOWN then it is
     *  automatically guessed */
    enum gmio_stl_format format_hint;

    /*! Restrict gmio_stl_infos_get() to not read further this limit(in bytes)
     *
     *  \warning Not yet supported
     */
    gmio_streamsize_t size_limit;
};

GMIO_C_LINKAGE_BEGIN

/*! Finds informations about STL contents
 *
 *  \p infos is an output parameter that will hold the retrieved infos
 *
 *  \p flags is a bitor combination of \c gmio_stl_info_flag values and is used
 *  to select the infos to retrieve.
 *
 *  \return Error code (see gmio_core/error.h and stl_error.h)
 */
GMIO_API int gmio_stl_infos_get(
                struct gmio_stl_infos* infos,
                struct gmio_stream* stream,
                unsigned flags,
                const struct gmio_stl_infos_get_options* options);

/*! Returns the size(in bytes) of the next STL ascii solid in \p stream
 *
 *  It is a facade over gmio_stl_infos_get() for gmio_stl_infos::size only
 *
 *  Pointer to this function can be given to
 *  gmio_stl_read_options::func_stla_get_streamsize() and is useful when
 *  reading in sequence multi solids in STL ascii. The stream can be cleanly
 *  advanced solid by solid after each call to gmio_stl_read()
 */
GMIO_API gmio_streamsize_t gmio_stla_infos_get_streamsize(
                struct gmio_stream* stream,
                struct gmio_memblock* stream_memblock);

GMIO_C_LINKAGE_END

#endif /* GMIO_STL_INFOS_H */
/*! @} */
