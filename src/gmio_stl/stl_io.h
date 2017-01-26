/****************************************************************************
** Copyright (c) 2017, Fougue Ltd. <http://www.fougue.pro>
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
**     1. Redistributions of source code must retain the above copyright
**        notice, this list of conditions and the following disclaimer.
**
**     2. Redistributions in binary form must reproduce the above
**        copyright notice, this list of conditions and the following
**        disclaimer in the documentation and/or other materials provided
**        with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
 *  The user mesh is created sequentially by calling
 *  gmio_stl_mesh_creator::func_add_triangle() with each triangle read from
 *  the stream.
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
 *
 *  \sa gmio_stl_read_file()
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
 *  \sa gmio_stl_read(), gmio_stream_stdio(FILE*)
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
 *
 *  \sa gmio_stl_read(), gmio_stl_read_file()
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
 *
 *  \sa gmio_stl_read(), gmio_stl_read_file()
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
 *
 *  \sa gmio_stl_write_file()
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
 *  \sa gmio_stl_write(), gmio_stream_stdio(FILE*)
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
