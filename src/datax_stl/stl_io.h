#ifndef FOUG_DATAX_LIBSTL_STL_IO_H
#define FOUG_DATAX_LIBSTL_STL_IO_H

#include "stl_global.h"
#include "stl_geom.h"
#include "stl_geom_creator.h"
#include "../datax_core/endian.h"
#include "../datax_core/transfer.h"

FOUG_C_LINKAGE_BEGIN

/*
 *  STL ascii
 */

/*! Read geometry from STL ascii stream */
FOUG_DATAX_LIBSTL_EXPORT int foug_stla_read(foug_stl_geom_creator_t* creator,
                                            foug_transfer_t*         trsf,
                                            size_t                   data_size_hint);

/*! Write geometry in the STL ascii format */
FOUG_DATAX_LIBSTL_EXPORT int foug_stla_write(const foug_stl_geom_t* geom,
                                             foug_transfer_t* trsf,
                                             const char*      solid_name,
                                             uint8_t          real32_prec);

/*
 *  STL binary
 */

/*! Read geometry from STL binary stream */
FOUG_DATAX_LIBSTL_EXPORT int foug_stlb_read(foug_stl_geom_creator_t* creator,
                                            foug_transfer_t*         trsf,
                                            foug_endianness_t        byte_order);

/*! Write geometry in the STL binary format */
FOUG_DATAX_LIBSTL_EXPORT int foug_stlb_write(const foug_stl_geom_t* geom,
                                             foug_transfer_t*       trsf,
                                             const uint8_t*         header_data,
                                             foug_endianness_t      byte_order);

FOUG_C_LINKAGE_END

#endif /* FOUG_DATAX_LIBSTL_STL_IO_H */
