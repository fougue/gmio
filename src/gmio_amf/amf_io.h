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

/*! \file amf_io.h
 *  AMF read/write functions
 *
 *  \addtogroup gmio_amf
 *  @{
 */

#pragma once

#include "amf_global.h"
#include "amf_document.h"
#include "amf_io_options.h"
#include "../gmio_core/stream.h"

GMIO_C_LINKAGE_BEGIN

/*! Writes AMF document to stream
 *
 *  \pre <tt> stream != NULL </tt>
 *  \pre <tt> doc != NULL </tt>
 *
 *  \p options may be \c NULL in this case default values are used
 *
 *  \return Error code (see gmio_core/error.h and amf_error.h)
 *
 *  \sa gmio_amf_write_file()
 */
GMIO_API int gmio_amf_write(
        struct gmio_stream* stream,
        const struct gmio_amf_document* doc,
        const struct gmio_amf_write_options* opts);

/*! Writes AMF document to stream
 *
 *  This is just a facility function over gmio_amf_write(). The internal stream
 *  object is created to read file at \p filepath
 *
 *  \pre <tt> filepath != \c NULL </tt>\n
 *       The file is opened with \c fopen() so \p filepath shall follow the file
 *       name specifications of the running environment
 *  \pre <tt> doc != NULL </tt>
 *
 *  \return Error code (see gmio_core/error.h and amf_error.h)
 *
 *  \sa gmio_amf_write(), gmio_stream_stdio(FILE*)
 */
GMIO_API int gmio_amf_write_file(
        const char* filepath,
        const struct gmio_amf_document* doc,
        const struct gmio_amf_write_options* opts);

GMIO_C_LINKAGE_END

/*! @} */
