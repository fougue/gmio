/****************************************************************************
** GeomIO Library
** Copyright Fougue (2 Mar. 2015)
** contact@fougsys.fr
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

/*! \file stl_io.h
 *  STL read/write functions
 */

#ifndef GMIO_STL_IO_H
#define GMIO_STL_IO_H

#include "stl_global.h"
#include "stl_format.h"
#include "stl_mesh.h"
#include "stl_mesh_creator.h"
#include "stl_io_options.h"
#include "../gmio_core/endian.h"
#include "../gmio_core/transfer.h"

GMIO_C_LINKAGE_BEGIN

/*! Reads STL mesh from file, format is automatically guessed
 *
 *  \param filepath Path to the STL file. A stream is opened with fopen() so
 *         the string has to be encoded using the system's charset (locale-8bit)
 *  \param creator Defines the callbacks for the mesh creation
 *  \param task_iface The interface object by which the read operation can be
 *         controlled. Optional, can be safely set to NULL
 *
 *  Internally, it uses:
 *    \li the builtin stream wrapper around FILE* (see gmio_stream_stdio())
 *    \li the global default function to construct a temporary gmio_buffer
 *        object (see gmio_buffer_default())
 *
 *  \return Error code (see error.h and stl_error.h)
 */
GMIO_LIBSTL_EXPORT
int gmio_stl_read_file(
        const char* filepath,
        gmio_stl_mesh_creator_t* creator,
        gmio_task_iface_t* task_iface);

/*! Reads STL mesh from stream, format is automatically guessed
 *
 *  \param trsf Defines needed objects for the read operation
 *  \param creator Defines the callbacks for the mesh creation
 *
 *  \return Error code (see error.h and stl_error.h)
 */
GMIO_LIBSTL_EXPORT
int gmio_stl_read(
        gmio_transfer_t* trsf,
        gmio_stl_mesh_creator_t* creator);

/*! Writes STL mesh to file
 *
 *  \param format STL format of the output file
 *  \param filepath Path to the STL file. A stream is opened with fopen() so
 *         the string has to be encoded using the system's charset (locale-8bit)
 *  \param mesh Defines the mesh to output
 *  \param task_iface The interface object by which the write operation can be
 *         controlled. Optional, can be safely set to NULL
 *  \param options Options for the write operation, can be safely set to NULL
 *         to use default values
 *
 *  Internally, it uses:
 *    \li the builtin stream wrapper around FILE* (see gmio_stream_stdio())
 *    \li the global default function to construct a temporary gmio_buffer
 *        object (see gmio_buffer_default())
 *
 *  \return Error code (see error.h and stl_error.h)
 */
GMIO_LIBSTL_EXPORT
int gmio_stl_write_file(
        gmio_stl_format_t format,
        const char* filepath,
        const gmio_stl_mesh_t* mesh,
        gmio_task_iface_t* task_iface,
        const gmio_stl_write_options_t* options);

/*! Writes STL mesh to stream
 *
 *  \param format STL format of the output
 *  \param trsf Defines needed objects for the write operation
 *  \param mesh Defines the mesh to output
 *  \param options Options for the write operation, can be safely set to NULL
 *         to use default values
 *
 *  \return Error code (see error.h and stl_error.h)
 */
GMIO_LIBSTL_EXPORT
int gmio_stl_write(
        gmio_stl_format_t format,
        gmio_transfer_t* trsf,
        const gmio_stl_mesh_t* mesh,
        const gmio_stl_write_options_t* options);

/*! Reads geometry from STL ascii stream
 *
 *  \param trsf Defines needed objects for the read operation
 *  \param creator Defines the callbacks for the mesh creation
 *
 *  Stream size is passed to gmio_task_iface::handle_progress_func() as the
 *  \p max_value argument.
 *
 *  Possible options in a future version could be:
 *     - flag to force locale ?
 *     - case sensitive/insensitive ?
 *
 *  \return Error code (see error.h and stl_error.h)
 */
GMIO_LIBSTL_EXPORT
int gmio_stla_read(
        gmio_transfer_t* trsf,
        gmio_stl_mesh_creator_t* creator);

/*! Reads geometry from STL binary stream
 *
 *  \param trsf Defines needed objects for the read operation
 *  \param creator Defines the callbacks for the mesh creation
 *  \param byte_order Byte order of the input STL binary data
 *
 *  \return Error code (see error.h and stl_error.h)
 *  \retval GMIO_ERROR_INVALID_BUFFER_SIZE
 *          if <tt>trsf->buffer.size < GMIO_STLB_MIN_CONTENTS_SIZE</tt>
 */
GMIO_LIBSTL_EXPORT
int gmio_stlb_read(
        gmio_transfer_t* trsf,
        gmio_stl_mesh_creator_t* creator,
        gmio_endianness_t byte_order);

GMIO_C_LINKAGE_END

#endif /* GMIO_STL_IO_H */
