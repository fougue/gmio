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

#pragma once

#include "stl_global.h"

#include "stl_format.h"
#include "stl_io_options.h"
#include "stl_mesh.h"
#include "stl_mesh_creator.h"
#include "../gmio_core/iodevice.h"
#include "../gmio_core/endian.h"

namespace gmio {

//! Reads STL mesh from I/O device, format is automatically guessed
//!
//! The user mesh is created sequentially by calling STL_MeshCreator::addTriangle()
//! with each triangle read from the device.
//!
//! It does nothing on the triangles read : no checking(eg. for Nan values),
//! normals are given as they are.
//!
//! \return Error code (see gmio_core/error.h and stl_error.h)
GMIO_API int STL_read(
                FuncReadData func_read,
                STL_MeshCreator* creator,
                STL_ReadOptions options = {});

//! Reads STL mesh from a file, format is automatically guessed
//!
//! The internal stream object is created to read file at 'filepath'
//!
//! The file is opened with \c fopen() so 'filepath' shall follow the file name
//! specifications of the running environment
//!
//! \return Error code (see gmio_core/error.h and stl_error.h)
//! \sa read()
GMIO_API int STL_read(
                const char* filepath,
                STL_MeshCreator* creator,
                STL_ReadOptions options = {});

//! Reads mesh from STL ascii input
//!
//! \return Error code (see gmio_core/error.h and stl_error.h)
//! \sa STL_read()
GMIO_API int STL_readAscii(
                FuncReadData func_read,
                STL_MeshCreator* creator,
                STL_ReadOptions options = {});

//! Reads mesh from STL binary input
//!
//! \return Error code (see gmio_core/error.h and stl_error.h)
//! \sa STL_read()
GMIO_API int STL_readBinary(
                Endianness byte_order,
                FuncReadData func_read,
                STL_MeshCreator* creator,
                STL_ReadOptions options = {});

//! Writes STL mesh to I/O device
//!
//! \return Error code (see gmio_core/error.h and stl_error.h)
GMIO_API int STL_write(
                STL_Format format,
                FuncWriteData func_write,
                const STL_Mesh& mesh,
                STL_WriteOptions options = {});

//! Writes STL mesh to stream
//!
//! The internal stream object is created to read file at 'filepath'
//!
//! The file is opened with \c fopen() so 'filepath' shall follow the file name
//! specifications of the running environment
//!
//! \return Error code (see gmio_core/error.h and stl_error.h)
GMIO_API int STL_write(
                STL_Format format,
                const char* filepath,
                const STL_Mesh& mesh,
                STL_WriteOptions options = {});

//! Writes STL binary header data to I/O device
//!
//! This functions only writes the 80-bytes header array and the count of facets
//! of the mesh(with respect of the specified byte order).
//!
//! \return Error code (see gmio_core/error.h and stl_error.h)
GMIO_API int STL_writeBinaryHeader(
                Endianness byte_order,
                FuncWriteData func_write,
                const STL_BinaryHeader& header,
                uint32_t facet_count);

} // namespace gmio

/*! @} */
