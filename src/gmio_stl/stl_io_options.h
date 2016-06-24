/****************************************************************************
** gmio
** Copyright Fougue (24 Jun. 2016)
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

/*! \file stl_io_options.h
 *  Options for STL read/write functions
 *
 *  \addtogroup gmio_stl
 *  @{
 */

#ifndef GMIO_STL_IO_OPTIONS_H
#define GMIO_STL_IO_OPTIONS_H

#include "stl_global.h"
#include "stlb_header.h"
#include "../gmio_core/endian.h"
#include "../gmio_core/stream.h"
#include "../gmio_core/task_iface.h"
#include "../gmio_core/text_format.h"

/*! Options of function gmio_stl_read() */
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
     *  \sa gmio_stla_infos_get_streamsize()
     */
    gmio_streamsize_t (*func_stla_get_streamsize)(
            struct gmio_stream* stream,
            struct gmio_memblock* stream_memblock);
};

/*! Options of function gmio_stl_write() */
struct gmio_stl_write_options
{
    /*! See gmio_stl_read_options::stream_memblock */
    struct gmio_memblock stream_memblock;

    /*! See gmio_stl_read_options::task_iface */
    struct gmio_task_iface task_iface;

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

#endif /* GMIO_STL_IO_OPTIONS_H */
/*! @} */
