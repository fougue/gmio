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

/*! \file stla_stats.h
 *  TODO: description
 *
 *  \addtogroup gmio_stl
 *  @{
 */

#ifndef GMIO_STLA_STATS_H
#define GMIO_STLA_STATS_H

#include "stl_global.h"

#include "../gmio_core/transfer.h"
#include "../gmio_core/internal/helper_stream.h"

#include <stddef.h>

/*! Statistics of some STL ascii solid */
struct gmio_stla_stats
{
    /*! Count of facets(triangles) */
    uint32_t facet_count;

    /*! Average facet size in bytes(what's in between facet ... endfacet
     *  including the tags) */
    size_t average_facet_size;

    /*! Size of the STL ascii contents in bytes, including the "endsolid" tag */
    size_t size;
};
typedef struct gmio_stla_stats gmio_stla_stats_t;

/*! Flags(OR-combinations) for each STL ascii statistic */
enum gmio_stla_stat_flag
{
    GMIO_STLA_STAT_FLAG_FACET_COUNT = 0x01,
    GMIO_STLA_STAT_FLAG_AVERAGE_FACET_SIZE = 0x02,
    GMIO_STLA_STAT_FLAG_SIZE = 0x04,
    GMIO_STLA_STAT_FLAG_ALL = 0xFF
};
typedef enum gmio_stla_stat_flag gmio_stla_stat_flag_t;

GMIO_C_LINKAGE_BEGIN

GMIO_LIBSTL_EXPORT
gmio_stla_stats_t gmio_stla_stats_get(
        gmio_transfer_t* trsf, unsigned stat_flags, size_t size_limit);

GMIO_C_LINKAGE_END

#endif /* GMIO_STLA_STATS_H */
/*! @} */
