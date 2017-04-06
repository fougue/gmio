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
#include "../gmio_core/stream.h"
#include "../gmio_core/task_iface.h"
#include "../gmio_core/text_format.h"

/*! Options of function gmio_stl_read()
 *
 *  Initialising gmio_stl_read_options with \c {0} (or \c {} in C++) is the
 *  convenient way to set default values(passing \c NULL to gmio_stl_read() has
 *  the same effect).
 */
struct gmio_stl_read_options
{
    /*! Used by the stream to bufferize I/O operations
     *
     *  If null, then a temporary memblock is created with the global default
     *  constructor function
     *
     *  \sa gmio_memblock_isnull()
     *  \sa gmio_memblock_default() */
    struct gmio_memblock stream_memblock;

    /*! Optional interface by which the I/O operation can be controlled */
    struct gmio_task_iface task_iface;

    /*! Optional pointer to a function that returns the size(in bytes) of the
     *  STL ascii data to read
     *
     *  Useful only with STL ascii format. If set to \c NULL then by default
     *  gmio_stream::func_size() is called.
     *
     *  The resulting stream size is passed to
     *  gmio_task_iface::func_handle_progress() as the \p max_value argument.
     *
     *  \sa gmio_stla_infos_probe_streamsize()
     */
    gmio_streamsize_t (*func_stla_get_streamsize)(
            struct gmio_stream* stream,
            struct gmio_memblock* stream_memblock);

    /*! Flag allowing to disable checking of the current locale's numeric
     *  formatting category
     *
     *  If \c false then \c LC_NUMERIC is checked to be "C" or "POSIX". If check
     *  fails then the function returns \c GMIO_ERROR_BAD_LC_NUMERIC
     *
     *  This applies only for STL ascii, where it affects text-related standard
     *  C functions(snprintf(), strtod(), ...)
     *
     *  \c LC_NUMERIC checking is enabled by default.
     */
    bool stla_dont_check_lc_numeric;
};

/*! Options of function gmio_stl_write()
 *
 *  Initialising gmio_stl_write_options with \c {0} (or \c {} in C++) is the
 *  convenient way to set default values(passing \c NULL to gmio_stl_write() has
 *  the same effect).
 */
struct gmio_stl_write_options
{
    /*! See gmio_stl_read_options::stream_memblock */
    struct gmio_memblock stream_memblock;

    /*! See gmio_stl_read_options::task_iface */
    struct gmio_task_iface task_iface;

    /*! See gmio_stl_read_options::stla_dont_check_lc_numeric */
    bool stla_dont_check_lc_numeric;

    /*! Flag allowing to skip writting of any header/footer data, but just
     *  triangles
     *
     *  If set to \c true then :
     *    \li for STL ASCII format, <tt>"solid [name]"</tt> and
     *        <tt>"endsolid"</tt> will no be written to output stream
     *    \li for STL binary format, the 80 bytes header followed by the mesh
     *        facet count (4bytes) will no be written to output stream
     */
    bool stl_write_triangles_only;

    /*! Name of the solid to appear in <tt>solid [name] facet normal ...</tt>
     *
     *  Option useful only with STL ascii format (GMIO_STL_FORMAT_ASCII).
     *
     *  Defaulted to an empty string \c "" when :
     *    \li calling gmio_stl_write() with <tt>options == NULL</tt>
     *    \li OR <tt>stla_solid_name == NULL</tt>
     */
    const char* stla_solid_name;

    /*! The format used when writting float values as strings
     *
     *  Option useful only with STL ascii format (GMIO_STL_FORMAT_ASCII).
     *
     *  Defaulted to \c GMIO_FLOAT_TEXT_FORMAT_DECIMAL_LOWERCASE when calling
     *  gmio_stl_write() with \c options==NULL
     */
    enum gmio_float_text_format stla_float32_format;

    /*! The maximum number of significant digits when writting float values
     *
     *  Option useful only with STL ascii format (GMIO_STL_FORMAT_ASCII).
     *
     *  Defaulted to \c 9 when calling gmio_stl_write() with \c options==NULL
     */
    uint8_t stla_float32_prec;

    /*! Header data(80-bytes)
     *
     *  Option useful only with STL binary formats (GMIO_STL_FORMAT_BINARY_LE
     *  or GMIO_STL_FORMAT_BINARY_BE).
     *
     *  Defaulted to an array containing 0 when :
     *    \li calling gmio_stl_write() with <tt>options == NULL</tt>
     *    \li OR <tt>stlb_header == NULL</tt>
     */
    struct gmio_stlb_header stlb_header;
};

/*! @} */
