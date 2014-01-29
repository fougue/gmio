#ifndef FOUG_DATAX_C_LIBSTL_STLB_WRITE_H
#define FOUG_DATAX_C_LIBSTL_STLB_WRITE_H

#include "stl_global.h"
#include "stl_geom.h"
#include "../endian.h"
#include "../transfer.h"

/* foug_stlb_write() */
FOUG_DATAX_LIBSTL_EXPORT int foug_stlb_write(const foug_stl_geom_t* geom,
                                             foug_transfer_t*       trsf,
                                             const uint8_t*         header_data,
                                             foug_endianness_t      byte_order);

#endif /* FOUG_DATAX_C_LIBSTL_STLB_WRITE_H */
