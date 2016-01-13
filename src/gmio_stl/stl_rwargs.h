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

/*! \file stl_rwargs.h
 *  Read/write structures for STL
 *
 *  \addtogroup gmio_stl
 *  @{
 */

#ifndef GMIO_STL_RWARGS_H
#define GMIO_STL_RWARGS_H

#include "stl_format.h"
#include "stl_global.h"
#include "stl_io_options.h"
#include "stl_mesh.h"
#include "stl_mesh_creator.h"
#include "../gmio_core/rwargs.h"

/*! Arguments for STL read functions */
struct gmio_stl_read_args
{
    /*! Common(core) objects needed for the read operation */
    struct gmio_rwargs core;

    /*! Defines the callbacks for the mesh creation */
    struct gmio_stl_mesh_creator mesh_creator;

    /*! Optional pointer to a function that returns the size(in bytes) of the
     *  STL ascii data to read
     *
     *  Useful only with STL ascii format. If set to NULL then by default
     *  gmio_stream::func_size() is called.
     *
     *  The resulting stream size is passed to
     *  gmio_task_iface::func_handle_progress() as the \p max_value argument.
     */
    gmio_streamsize_t (*func_stla_get_streamsize)(
            struct gmio_stream* stream,
            struct gmio_memblock* stream_memblock);
};

/*! Arguments for STL write functions */
struct gmio_stl_write_args
{
    /*! Common(core) objects needed for the write operation */
    struct gmio_rwargs core;

    /*! Defines the mesh to output */
    struct gmio_stl_mesh mesh;

    /*! Options for the write operation, can be safely set to \c {0} to use
     *  default values */
    struct gmio_stl_write_options options;
};

#endif /* GMIO_STL_RWARGS_H */
