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
#include "stl_io_options.h"
#include "stl_mesh.h"
#include "stl_mesh_creator.h"
#include "../gmio_core/stream.h"
#include "../gmio_core/endian.h"

GMIO_C_LINKAGE_BEGIN

/*! Reads STL mesh from stream, format is automatically guessed
 *
 *  \return Error code (see gmio_core/error.h and stl_error.h)
 */
GMIO_LIBSTL_EXPORT int gmio_stl_read(
        struct gmio_stream* stream,
        struct gmio_stl_mesh_creator* mesh_creator,
        const struct gmio_stl_read_options* options);

/*! Reads STL mesh from a file, format is automatically guessed
 *
 *  This is just a facility function over gmio_stl_read(). The internal stream
 *  object is created to read file at \p filepath (see gmio_stream_stdio(FILE*))
 *
 *  The file is opened with fopen() so \p filepath shall follow the file name
 *  specifications of the running environment
 */
GMIO_LIBSTL_EXPORT int gmio_stl_read_file(
        const char* filepath,
        struct gmio_stl_mesh_creator* mesh_creator,
        const struct gmio_stl_read_options* options);

/*! Reads geometry from STL ascii stream
 *
 *  \return Error code (see gmio_core/error.h and stl_error.h)
 */
GMIO_LIBSTL_EXPORT
int gmio_stla_read(
        struct gmio_stream* stream,
        struct gmio_stl_mesh_creator* mesh_creator,
        const struct gmio_stl_read_options* options);

/*! Reads geometry from STL binary stream
 *
 *  \return Error code (see gmio_core/error.h and stl_error.h)
 *  \retval GMIO_ERROR_INVALID_MEMBLOCK_SIZE
 *          if <tt>options->stream_memblock.size < GMIO_STLB_MIN_CONTENTS_SIZE</tt>
 */
GMIO_LIBSTL_EXPORT
int gmio_stlb_read(
        struct gmio_stream* stream,
        struct gmio_stl_mesh_creator* mesh_creator,
        enum gmio_endianness byte_order,
        const struct gmio_stl_read_options* options);

/*! Writes STL mesh to stream
 *
 *  \return Error code (see gmio_core/error.h and stl_error.h)
 */
GMIO_LIBSTL_EXPORT
int gmio_stl_write(
        enum gmio_stl_format format,
        struct gmio_stream* stream,
        const struct gmio_stl_mesh* mesh,
        const struct gmio_stl_write_options* options);

/*! Writes STL mesh to stream
 *
 *  This is just a facility function over gmio_stl_write(). The internal stream
 *  object is created to read file at \p filepath (see gmio_stream_stdio(FILE*))
 *
 *  The file is opened with fopen() so \p filepath shall follow the file name
 *  specifications of the running environment

 *  \return Error code (see gmio_core/error.h and stl_error.h)
 */
GMIO_LIBSTL_EXPORT
int gmio_stl_write_file(
        enum gmio_stl_format format,
        const char* filepath,
        const struct gmio_stl_mesh* mesh,
        const struct gmio_stl_write_options* options);

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
