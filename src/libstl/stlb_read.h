#ifndef FOUG_DATAX_C_LIBSTL_STLB_READ_H
#define FOUG_DATAX_C_LIBSTL_STLB_READ_H

#include "stl_global.h"
#include "stl_geom_creator.h"
#include "../endian.h"
#include "../transfer.h"

/* foug_stlb_read() */
FOUG_DATAX_LIBSTL_EXPORT int foug_stlb_read(foug_stl_geom_creator_t* creator,
                                            foug_transfer_t*         trsf,
                                            foug_endianness_t        byte_order);

#endif /* FOUG_DATAX_C_LIBSTL_STLB_READ_H */
