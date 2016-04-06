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
 *  It does nothing on the triangles read : no checking(eg. for Nan values),
 *  normals are given as they are.
 *
 *  \pre <tt> stream != NULL </tt>
 *  \pre <tt> mesh_creator != NULL </tt>
 *
 *  \p options may be \c NULL in this case default values are used
 *
 *  \return Error code (see gmio_core/error.h and stl_error.h)
 */
GMIO_API int gmio_stl_read(
                struct gmio_stream* stream,
                struct gmio_stl_mesh_creator* mesh_creator,
                const struct gmio_stl_read_options* options);

/*! Reads STL mesh from a file, format is automatically guessed
 *
 *  This is just a facility function over gmio_stl_read(). The internal stream
 *  object is created to read file at \p filepath.
 *
 *  \pre <tt> filepath != \c NULL </tt>\n
 *       The file is opened with \c fopen() so \p filepath shall follow the file
 *       name specifications of the running environment
 *  \pre <tt> mesh_creator != NULL </tt>
 *
 *  \p options may be \c NULL in this case default values are used
 *
 *  \return Error code (see gmio_core/error.h and stl_error.h)
 *
 *  \sa gmio_stream_stdio(FILE*)
 */
GMIO_API int gmio_stl_read_file(
                const char* filepath,
                struct gmio_stl_mesh_creator* mesh_creator,
                const struct gmio_stl_read_options* options);

/*! Reads mesh from STL ascii stream
 *
 *  \pre <tt> stream != NULL </tt>
 *  \pre <tt> mesh_creator != NULL </tt>
 *
 *  \p options may be \c NULL in this case default values are used
 *
 *  \return Error code (see gmio_core/error.h and stl_error.h)
 */
GMIO_API int gmio_stla_read(
                struct gmio_stream* stream,
                struct gmio_stl_mesh_creator* mesh_creator,
                const struct gmio_stl_read_options* options);

/*! Reads mesh from STL binary stream
 *
 *  \pre <tt> stream != NULL </tt>
 *  \pre <tt> mesh_creator != NULL </tt>
 *
 *  \p options may be \c NULL in this case default values are used
 *
 *  \return Error code (see gmio_core/error.h and stl_error.h)
 *  \retval GMIO_ERROR_INVALID_MEMBLOCK_SIZE
 *          if <tt>options->stream_memblock.size < GMIO_STLB_MIN_CONTENTS_SIZE</tt>
 */
GMIO_API int gmio_stlb_read(
                struct gmio_stream* stream,
                struct gmio_stl_mesh_creator* mesh_creator,
                enum gmio_endianness byte_order,
                const struct gmio_stl_read_options* options);

/*! Writes STL mesh to stream
 *
 *  \pre <tt> stream != NULL </tt>
 *  \pre <tt> mesh != NULL </tt>
 *
 *  \p options may be \c NULL in this case default values are used
 *
 *  \return Error code (see gmio_core/error.h and stl_error.h)
 */
GMIO_API int gmio_stl_write(
                enum gmio_stl_format format,
                struct gmio_stream* stream,
                const struct gmio_stl_mesh* mesh,
                const struct gmio_stl_write_options* options);

/*! Writes STL mesh to stream
 *
 *  This is just a facility function over gmio_stl_write(). The internal stream
 *  object is created to read file at \p filepath
 *
 *  \pre <tt> filepath != \c NULL </tt>\n
 *       The file is opened with \c fopen() so \p filepath shall follow the file
 *       name specifications of the running environment
 *  \pre <tt> mesh != NULL </tt>
 *
 *  \return Error code (see gmio_core/error.h and stl_error.h)
 *
 *  \sa gmio_stream_stdio(FILE*)
 */
GMIO_API int gmio_stl_write_file(
                enum gmio_stl_format format,
                const char* filepath,
                const struct gmio_stl_mesh* mesh,
                const struct gmio_stl_write_options* options);

/*! Writes STL binary header data to stream
 *
 *  This functions only writes the 80-bytes header array and the count of facets
 *  of the mesh(with respect of the specified byte order).
 *
 *  \pre <tt> stream != NULL </tt>
 *
 *  \p header Can be safely set to \c NULL to generate an array of zeroes
 *
 *  \return Error code (see gmio_core/error.h and stl_error.h)
 */
GMIO_API int gmio_stlb_header_write(
                struct gmio_stream* stream,
                enum gmio_endianness byte_order,
                const struct gmio_stlb_header* header,
                uint32_t facet_count
);

GMIO_C_LINKAGE_END

#endif /* GMIO_STL_IO_H */
/*! @} */
