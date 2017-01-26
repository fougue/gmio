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

#ifndef GMIO_INTERNAL_STLB_WRITE_H
#define GMIO_INTERNAL_STLB_WRITE_H

#include "../../gmio_core/endian.h"
#include "../../gmio_core/stream.h"
#include "../stl_io_options.h"
#include "../stl_mesh.h"

/*! Writes geometry in the STL binary format
 *
 *  \return Error code (see error.h and stl_error.h)
 *  \retval GMIO_INVALID_MEMBLOCK_SIZE_ERROR
 *          if <tt>opts->stream_memblock.size < GMIO_STLB_MIN_CONTENTS_SIZE</tt>
 */
int gmio_stlb_write(
        enum gmio_endianness byte_order,
        struct gmio_stream* stream,
        const struct gmio_stl_mesh* mesh,
        const struct gmio_stl_write_options* opts);

#endif /* GMIO_INTERNAL_STLB_WRITE_H */
