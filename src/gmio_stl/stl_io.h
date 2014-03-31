#ifndef GMIO_LIBSTL_STL_IO_H
#define GMIO_LIBSTL_STL_IO_H

#include "stl_global.h"
#include "stl_mesh.h"
#include "stl_mesh_creator.h"
#include "../gmio_core/endian.h"
#include "../gmio_core/transfer.h"

GMIO_C_LINKAGE_BEGIN

/*
 *  STL ascii
 */

/*! Read geometry from STL ascii stream */
GMIO_LIBSTL_EXPORT int gmio_stla_read(gmio_stl_mesh_creator_t* creator,
                                      gmio_transfer_t*         trsf,
                                      size_t                   data_size_hint);

/*! Write geometry in the STL ascii format */
GMIO_LIBSTL_EXPORT int gmio_stla_write(const gmio_stl_mesh_t* mesh,
                                       gmio_transfer_t* trsf,
                                       const char*      solid_name,
                                       uint8_t          real32_prec);

/*
 *  STL binary
 */

/*! Read geometry from STL binary stream */
GMIO_LIBSTL_EXPORT int gmio_stlb_read(gmio_stl_mesh_creator_t* creator,
                                      gmio_transfer_t*         trsf,
                                      gmio_endianness_t        byte_order);

/*! Write geometry in the STL binary format */
GMIO_LIBSTL_EXPORT int gmio_stlb_write(const gmio_stl_mesh_t* mesh,
                                       gmio_transfer_t*       trsf,
                                       const uint8_t*         header_data,
                                       gmio_endianness_t      byte_order);

GMIO_C_LINKAGE_END

#endif /* GMIO_LIBSTL_STL_IO_H */
