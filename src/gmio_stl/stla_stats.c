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

#include "stla_stats.h"

#include "../gmio_core/error.h"
#include "../gmio_core/internal/helper_stream.h"
#include "../gmio_core/internal/helper_transfer.h"
#include "../gmio_stl/internal/stl_rw_common.h"

gmio_stla_stats_t gmio_stla_stats_get(
        gmio_transfer_t* trsf, unsigned stat_flags, size_t size_limit)
{
    gmio_stla_stats_t stats = {0};
    void* mblock_ptr = trsf != NULL ? trsf->memblock.ptr : NULL;
    const size_t mblock_size = trsf != NULL ? trsf->memblock.size : NULL;
    int err = GMIO_ERROR_OK;

    /* Check validity of input transfer object */
    if (!gmio_check_transfer(&err, trsf))
        return stats;

    /*if (stat_flags != 0) {
        while (gmio_no_error(err)) {
            const size_t read_size =
                    gmio_stream_read(stream, buffer_ptr, 1, buffer_size);
            const int stream_err = gmio_stream_error(trsf->stream);

            err = stream_err;
        }
    }*/

    return stats;
}
