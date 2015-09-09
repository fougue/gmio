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

/*! \file stl_io.h
 *  STL read/write functions
 *
 *  \addtogroup gmio_stl
 *  @{
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
 *  Internally, it uses:
 *    \li the builtin stream wrapper around FILE* (see gmio_stream_stdio())
 *    \li the global default function to construct a temporary gmio_buffer
 *        object (see gmio_buffer_default())
 *
 *  \return Error code (see error.h and stl_error.h)
 */
GMIO_LIBSTL_EXPORT
int gmio_stl_read_file(
        /*! Path to the STL file.
         *  A stream is opened with fopen() so the string shall follow the file
         *  name specifications of the running environment */
        const char* filepath,

        /*! Defines the callbacks for the mesh creation */
        gmio_stl_mesh_creator_t* creator,

        /*! The interface object by which the read operation can be controlled.
         *  Optional, can be safely set to NULL */
        gmio_task_iface_t* task_iface
);

/*! Reads STL mesh from stream, format is automatically guessed
 *
 *  \return Error code (see error.h and stl_error.h)
 */
GMIO_LIBSTL_EXPORT
int gmio_stl_read(
        /*! Defines needed objects for the read operation */
        gmio_transfer_t* trsf,

        /*! Defines the callbacks for the mesh creation */
        gmio_stl_mesh_creator_t* creator
);

/*! Writes STL mesh to file
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
        /*! STL format of the output file */
        gmio_stl_format_t format,

        /*! Path to the STL file. A stream is opened with fopen() so the string
         *  shall follow the file name specifications of the running
         *  environment */
        const char* filepath,

        /*! Defines the mesh to output */
        const gmio_stl_mesh_t* mesh,

        /*! The interface object by which the write operation can be controlled.
         *  Optional, can be safely set to NULL */
        gmio_task_iface_t* task_iface,

        /*! Options for the write operation, can be safely set to NULL to use
         *  default values */
        const gmio_stl_write_options_t* options
);

/*! Writes STL mesh to stream
 *
 *  \return Error code (see error.h and stl_error.h)
 */
GMIO_LIBSTL_EXPORT
int gmio_stl_write(
        /*! STL format of the output */
        gmio_stl_format_t format,

        /*! Defines needed objects for the write operation */
        gmio_transfer_t* trsf,

        /*! Defines the mesh to output */
        const gmio_stl_mesh_t* mesh,

        /*! Options for the write operation, can be safely set to NULL to use
         *  default values */
        const gmio_stl_write_options_t* options
);

/*! Reads geometry from STL ascii stream
 *
 *  Stream size is passed to gmio_task_iface::func_handle_progress() as the
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
        /*! Defines needed objects for the read operation */
        gmio_transfer_t* trsf,

        /*! Defines the callbacks for the mesh creation */
        gmio_stl_mesh_creator_t* creator
);

/*! Size(in bytes) of the minimum contents possible with the STL binary format */
enum { GMIO_STLB_MIN_CONTENTS_SIZE = 284 };

/*! Reads geometry from STL binary stream
 *
 *  \return Error code (see error.h and stl_error.h)
 *  \retval GMIO_ERROR_INVALID_BUFFER_SIZE
 *          if <tt>trsf->buffer.size < GMIO_STLB_MIN_CONTENTS_SIZE</tt>
 */
GMIO_LIBSTL_EXPORT
int gmio_stlb_read(
        /*! Defines needed objects for the read operation */
        gmio_transfer_t* trsf,

        /*! Defines the callbacks for the mesh creation */
        gmio_stl_mesh_creator_t* creator,

        /*! Byte order of the input STL binary data */
        gmio_endianness_t byte_order
);

/*! Writes STL binary header data to stream
 *
 *  This functions only writes the 80-bytes header array and the count of facets
 *  in the mesh(with respect of the specified byte order).
 *
 *  \return Error code (see error.h and stl_error.h)
 */
GMIO_LIBSTL_EXPORT
int gmio_stlb_write_header(
        /*! Output stream where is written the header data */
        gmio_stream_t* stream,

        /*! Byte order of the output STL data */
        gmio_endianness_t byte_order,

        /*! 80-bytes array of header data, can be safely set to NULL to generate
         * an array of zeroes */
        const gmio_stlb_header_t* header,

        /*! Total count of facets (triangles) in the mesh to be written */
        uint32_t facet_count
);

GMIO_C_LINKAGE_END

#endif /* GMIO_STL_IO_H */
/*! @} */
