#ifndef FOUG_DATAX_C_LIBSTL_STLA_READ_H
#define FOUG_DATAX_C_LIBSTL_STLA_READ_H

#include "stl_global.h"
#include "stl_geom_creator.h"
#include "../transfer.h"

/* foug_stla_read() */
FOUG_DATAX_LIBSTL_EXPORT int foug_stla_read(foug_stl_geom_creator_t* creator,
                                            foug_transfer_t*         trsf,
                                            size_t                   data_size_hint);

#endif /* FOUG_DATAX_C_LIBSTL_STLA_READ_H */
