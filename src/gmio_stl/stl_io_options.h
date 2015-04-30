/****************************************************************************
** GeomIO Library
** Copyright Fougue (2 Mar. 2015)
** contact@fougsys.fr
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
 */

#ifndef GMIO_STL_IO_OPTIONS_H
#define GMIO_STL_IO_OPTIONS_H

#include "stl_global.h"
#include "../gmio_core/endian.h"
#include "../gmio_core/text_format.h"

/*! Options for gmio_stl_write() */
struct gmio_stl_write_options
{
    /*! Name of the solid to appear in "solid <name> \n facet normal ..."
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
    gmio_float_text_format_t stla_float32_format;

    /*! The maximum number of significant digits when writting float values
     *
     *  Option useful only with STL ascii format (GMIO_STL_FORMAT_ASCII).
     *
     *  Defaulted to \c 9 when calling gmio_stl_write() with \c options==NULL
     */
    uint8_t stla_float32_prec;

    /*! Header data consisting of 80 bytes
     *
     *  Option useful only with STL binary formats (GMIO_STL_FORMAT_BINARY_LE
     *  or GMIO_STL_FORMAT_BINARY_BE).
     *
     *  Defaulted to an array containing 0 when :
     *    \li calling gmio_stl_write() with <tt>options == NULL</tt>
     *    \li OR <tt>stlb_header_data == NULL</tt>
     */
    const uint8_t* stlb_header_data;
};
typedef struct gmio_stl_write_options gmio_stl_write_options_t;

#endif /* GMIO_STL_IO_OPTIONS_H */
