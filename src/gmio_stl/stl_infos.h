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
 *  TODO: description
 *
 *  \addtogroup gmio_stl
 *  @{
 */

#ifndef GMIO_STL_INFOS_H
#define GMIO_STL_INFOS_H

#include "stl_global.h"

#include "../gmio_core/rwargs.h"
#include "../gmio_core/internal/helper_stream.h"

#include <stddef.h>


#include "stl_format.h"
#include "stlb_header.h"

struct gmio_stl_infos
{
    /*! Count of facets(triangles) */
    uint32_t facet_count;

    /*! Size of the STL data in bytes
     *
     *  For STL ascii it includes the "endsolid" tag */
    size_t size;

    /*! STL ascii only: name of the solid, the pointer has to be set by the
     *  caller of gmio_stl_infos_get() */
    char* stla_solidname;

    /*! STL ascii only: maximum length(capacity) of stla_solidname, it has to be
     *  set by the caller of gmio_stl_infos_get()
     */
    size_t stla_solidname_maxlen;

    /*! STL binary only */
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

    /*! All infos */
    GMIO_STL_INFO_FLAG_ALL = 0xFFFF
};

struct gmio_stl_infos_get_args
{
    enum gmio_stl_format format;
    struct gmio_stream stream;
    struct gmio_memblock memblock;
};

GMIO_C_LINKAGE_BEGIN

GMIO_LIBSTL_EXPORT
int gmio_stl_infos_get(
        struct gmio_stl_infos_get_args* args,
        struct gmio_stl_infos* infos,
        unsigned flags);

GMIO_C_LINKAGE_END

#endif /* GMIO_STL_INFOS_H */
/*! @} */
