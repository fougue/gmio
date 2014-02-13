#ifndef FOUG_DATAX_C_LIBSTL_STLA_WRITE_H
#define FOUG_DATAX_C_LIBSTL_STLA_WRITE_H

#include "stl_global.h"
#include "stl_geom.h"
#include "../transfer.h"

/*! Write geometry in the STL ascii format */
FOUG_DATAX_LIBSTL_EXPORT int foug_stla_write(foug_stl_geom_t* geom,
                                             foug_transfer_t* trsf,
                                             const char*      solid_name,
                                             uint8_t          real32_prec);

#endif /* FOUG_DATAX_C_LIBSTL_STLA_WRITE_H */
