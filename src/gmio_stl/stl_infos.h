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

/*! \file stl_infos.h
 *  Retrieval of STL infos from input stream
 *
 *  \addtogroup gmio_stl
 *  @{
 */

#pragma once

#include "stl_global.h"
#include "stl_format.h"
#include "stlb_header.h"
#include "../gmio_core/iodevice.h"
#include "../gmio_core/result.h"
#include "../gmio_core/span.h"
#include <string>

namespace gmio {

//! Informations retrieved by STL_probeInfos()
struct STL_Infos {
    STL_Format format;    //!< Format of the input
    uint32_t facet_count; //!< Count of facets(triangles)
    uint64_t size;        //!< Size of the STL data in bytes
    std::string ascii_solid_name;   //!< STL ascii only: name of the solid
    STL_BinaryHeader binary_header; //!< STL binary only: header(80-bytes)
};

//! Flags(OR-combinations) for each STL info
enum STL_InfoFlag {
    STL_InfoFlag_FacetCount = 0x0001,     //! -> STL_Infos::facet_count
    STL_InfoFlag_Size = 0x0002,           //! -> STL_Infos::size
    STL_InfoFlag_AsciiSolidName = 0x0004, //! -> STL_Infos::ascii_solidname
    STL_InfoFlag_BinaryHeader = 0x0008,   //! -> STL_Infos::binary_header
    STL_InfoFlag_SolidNameOrHeader =
        STL_InfoFlag_AsciiSolidName | STL_InfoFlag_BinaryHeader,
    STL_InfoFlag_Format = 0x0010, //! -> STL_Infos::format
    STL_InfoFlag_All = 0xFFFF     //! -> All infos
};

//! Options of function STL_probeInfos()
struct STL_ProbeInfosOptions {
    Span<uint8_t> buffer;

    //! Assume input format, if STL_Format_Unknown then format is guessed
    STL_Format format_hint;

    //! Restrict STL_probeInfos() to not read further this limit(in bytes)
    //! \warning Not yet supported
    uint64_t size_limit;
};

//! Finds informations about STL contents
//!
//! 'flags' is a bitor combination of \c STL_InfoFlag values and is used
//! to select the informations to retrieve.
GMIO_API Result<STL_Infos> STL_probeInfos(
                FuncReadData func_read,
                unsigned flags,
                STL_ProbeInfosOptions options = {});

//! Finds informations about STL contents from a file
//!
//! The internal object is created to read file at 'filepath'.
//!
//! The file is opened with \c fopen() so 'filepath' shall follow the file name
//! specifications of the running environment
GMIO_API Result<STL_Infos> STL_probeInfos(
                const char* filepath,
                unsigned flags,
                STL_ProbeInfosOptions options = {});

//! Returns the size(in bytes) of the STL ascii solid read with 'funcRead'
//!
//! It is a facade over STL_probeInfos() for STL_Infos::size only
//!
//! Result of this function can be given to STL_ReadOptions::ascii_solid_size
//! and is useful when reading in sequence multi solids
GMIO_API uint64_t STL_probeAsciiSolidSize(FuncReadData func_read);

} // namespace gmio

/*! @} */
