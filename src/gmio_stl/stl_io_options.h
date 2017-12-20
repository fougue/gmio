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

/*! \file stl_io_options.h
 *  Options for STL read/write functions
 *
 *  \addtogroup gmio_stl
 *  @{
 */

#pragma once

#include "stl_global.h"
#include "stlb_header.h"
#include "../gmio_core/endian.h"
#include "../gmio_core/iodevice.h"
#include "../gmio_core/span.h"
#include "../gmio_core/task.h"
#include "../gmio_core/text_format.h"

#include <functional>
#include <string>

namespace gmio {

//! Options of function STL_read()
//!
//! Initialising STL_ReadOptions with \c {} in C++ is the convenient way to set
//! default values
struct STL_ReadOptions {
    Span<uint8_t> buffer;
    size_t buffer_initial_contents_size;

    //! Optional interface by which the I/O operation can be controlled
    Task* task;

    //! Optional size(in bytes) of the STL ascii data to read
    //!
    //! Useful only with STL ascii format, the solid size is passed to
    //! Task::handleProgress() as the 'max_value' argument.
    //!
    //! \sa STL_probeAsciiSolidSize()
    //! \sa FILE_size()
    uint64_t ascii_solid_size;

    //! Flag allowing to enable checking of the current locale's numeric
    //! formatting category
    //!
    //! If \c false then \c LC_NUMERIC is checked to be "C" or "POSIX". If check
    //! fails then the function returns \c ERROR_BAD_LC_NUMERIC
    //!
    //! This applies only for STL ascii, where it affects text-related standard
    //! C functions(snprintf(), strtod(), ...)
    //!
    //! \c LC_NUMERIC checking is enabled by default.
    bool ascii_dont_check_lc_numeric;
};


//! Options of function STL_write()
//!
//! Initialising STL_WriteOptions with \c {} in C++ is the convenient way to set
//! default values
struct STL_WriteOptions {
    Span<uint8_t> buffer;

    //! See STL_ReadOptions::task
    Task* task;

    //! See STL_ReadOptions::ascii_dont_check_lc_numeric
    bool ascii_dont_check_lc_numeric;

    //! Name of the solid to appear in <tt>solid [name] facet normal ...</tt>
    //!
    //! Option useful only with STL ascii format
    std::string ascii_solid_name;

    //! The format used when writting float values as strings
    //!
    //! Option useful only with STL ascii format
    //!
    //! Defaults to \c FloatTextFormat::DecimalLowercase
    FloatTextFormat ascii_float32_format;

    //! The maximum number of significant digits when writting float values
    //!
    //! Option useful only with STL ascii format
    //!
    //! Defaults to \c 9
    uint8_t ascii_float32_prec;

    //! Flag allowing to skip writting of any header/footer data, but just
    //! triangles
    //!
    //! If set to \c true then :
    //!   \li for STL ASCII format, <tt>"solid [name]"</tt> and
    //!       <tt>"endsolid"</tt> will no be written to output stream
    //!   \li for STL binary format, the 80 bytes header followed by the mesh
    //!       facet count (4bytes) will no be written to output stream
    bool write_triangles_only;

    //! Header data(80-bytes)
    //!
    //! Option useful only with STL binary formats
    STL_BinaryHeader binary_header;
};

} // namespace gmio

/*! @} */
