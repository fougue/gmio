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
#include "../gmio_core/endian.h"

struct gmio_rwargs;
struct gmio_stream;
struct gmio_stl_mesh;
struct gmio_stl_mesh_creator;
struct gmio_stl_write_options;
struct gmio_stlb_header;

GMIO_C_LINKAGE_BEGIN

/*! Reads STL mesh from file, format is automatically guessed
 *
 *  \return Error code (see error.h and stl_error.h)
 */
GMIO_LIBSTL_EXPORT
int gmio_stl_read_file(
        /*! Path to the STL file.
         *  A stream is opened with fopen() so the string shall follow the file
         *  name specifications of the running environment */
        const char* filepath,

        /*! Common objects needed for the read operation
         *  gmio_read_args::stream is internally initialized with the
         *  builtin stream wrapper around \c FILE* (see gmio_stream_stdio()) */
        struct gmio_rwargs* args,

        /*! Defines the callbacks for the mesh creation */
        struct gmio_stl_mesh_creator* creator
);

/*! Reads STL mesh from stream, format is automatically guessed
 *
 *  \return Error code (see error.h and stl_error.h)
 */
GMIO_LIBSTL_EXPORT
int gmio_stl_read(
        /*! Common objects needed for the read operation */
        struct gmio_rwargs* args,

        /*! Defines the callbacks for the mesh creation */
        struct gmio_stl_mesh_creator* creator
);

/*! Writes STL mesh to file
 *
 *  Internally, it uses:
 *    \li the builtin stream wrapper around FILE* (see gmio_stream_stdio())
 *    \li the global default function to construct a temporary gmio_memblock
 *        object (see gmio_memblock_default())
 *
 *  \return Error code (see error.h and stl_error.h)
 */
GMIO_LIBSTL_EXPORT
int gmio_stl_write_file(
        /*! Path to the STL file. A stream is opened with fopen() so the string
         *  shall follow the file name specifications of the running
         *  environment */
        const char* filepath,

        /*! Common objects needed for the write operation
         *  gmio_read_args::stream is internally initialized with the
         *  builtin stream wrapper around \c FILE* (see gmio_stream_stdio()) */
        struct gmio_rwargs* args,

        /*! Defines the mesh to output */
        const struct gmio_stl_mesh* mesh,

        /*! STL format of the output file */
        enum gmio_stl_format format,

        /*! Options for the write operation, can be safely set to NULL to use
         *  default values */
        const struct gmio_stl_write_options* options
);

/*! Writes STL mesh to stream
 *
 *  \return Error code (see error.h and stl_error.h)
 */
GMIO_LIBSTL_EXPORT
int gmio_stl_write(
        /*! Common objects needed for the write operation */
        struct gmio_rwargs* args,

        /*! Defines the mesh to output */
        const struct gmio_stl_mesh* mesh,

        /*! STL format of the output */
        enum gmio_stl_format format,

        /*! Options for the write operation, can be safely set to NULL to use
         *  default values */
        const struct gmio_stl_write_options* options
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
        /*! Common objects needed for the read operation */
        struct gmio_rwargs* args,

        /*! Defines the callbacks for the mesh creation */
        struct gmio_stl_mesh_creator* creator
);

/*! Size(in bytes) of the minimum contents possible with the STL binary format */
enum { GMIO_STLB_MIN_CONTENTS_SIZE = 284 };

/*! Reads geometry from STL binary stream
 *
 *  \return Error code (see error.h and stl_error.h)
 *  \retval GMIO_ERROR_INVALID_MEMBLOCK_SIZE
 *          if <tt>trsf->memblock.size < GMIO_STLB_MIN_CONTENTS_SIZE</tt>
 */
GMIO_LIBSTL_EXPORT
int gmio_stlb_read(
        /*! Common objects needed for the read operation */
        struct gmio_rwargs* args,

        /*! Defines the callbacks for the mesh creation */
        struct gmio_stl_mesh_creator* creator,

        /*! Byte order of the input STL binary data */
        enum gmio_endianness byte_order
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
        struct gmio_stream* stream,

        /*! Byte order of the output STL data */
        enum gmio_endianness byte_order,

        /*! 80-bytes array of header data, can be safely set to NULL to generate
         * an array of zeroes */
        const struct gmio_stlb_header* header,

        /*! Total count of facets (triangles) in the mesh to be written */
        uint32_t facet_count
);

GMIO_C_LINKAGE_END

#endif /* GMIO_STL_IO_H */
/*! @} */
