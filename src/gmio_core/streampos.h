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

/*! \file streampos.h
 *  Declaration of gmio_streampos and utility functions
 *
 *  \addtogroup gmio_core
 *  @{
 */

#pragma once

#include "global.h"

/*! \c GMIO_STREAMPOS_COOKIE_SIZE
 *  Size of the byte array gmio_streampos::cookie */
enum { GMIO_STREAMPOS_COOKIE_SIZE = 32 /* bytes */ };

/*! Specifies a position within a stream
 *
 *  The information in gmio_streampos objects is usually filled by a call to
 *  gmio_stream::func_get_pos(), which takes a pointer to an object of this type
 *  as argument.
 *
 *  The content of a gmio_streampos object is not meant to be read directly,
 *  but only to be used as an argument in a call to gmio_stream::func_set_pos()
 */
struct gmio_streampos
{
    /*! Stores the actual(concrete) stream position object */
    uint8_t cookie[GMIO_STREAMPOS_COOKIE_SIZE];
};

/*! @} */
