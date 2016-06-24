/****************************************************************************
** gmio
** Copyright Fougue (24 Jun. 2016)
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

#ifndef GMIO_INTERNAL_STLA_WRITE_H
#define GMIO_INTERNAL_STLA_WRITE_H

#include "../../gmio_core/stream.h"
#include "../stl_io_options.h"
#include "../stl_mesh.h"

/*! Writes geometry in the STL ascii format
 *
 *  \return Error code (see gmio_core/error.h and stl_error.h)
 *  \retval GMIO_ERROR_INVALID_MEMBLOCK_SIZE
 *          if <tt>options->stream_memblock.size < 512</tt>
 */
int gmio_stla_write(
        struct gmio_stream* stream,
        const struct gmio_stl_mesh* mesh,
        const struct gmio_stl_write_options* options);

#endif /* GMIO_INTERNAL_STLA_WRITE_H */
