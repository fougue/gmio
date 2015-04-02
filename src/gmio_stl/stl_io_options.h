/****************************************************************************
** GeomIO Library
** Copyright FougSys (2 Mar. 2015)
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

/*! Options for gmio_stla_write() */
struct gmio_stla_write_options
{
    /*! Name of the solid to appear in "solid <name> \n facet normal ..."
     *
     * Defaulted to an empty string "" when :
     *    \li calling gmio_stla_write() with <tt>options == NULL</tt>
     *    \li OR <tt>solid_name == NULL</tt>
     */
    const char* solid_name;

    /*! The maximum number of significant digits to write float values
     *
     *  Defaulted to \c 9 when calling gmio_stla_write() with \c options==NULL
     */
    uint8_t float32_prec;
};
typedef struct gmio_stla_write_options  gmio_stla_write_options_t;

/*! Options for gmio_stlb_write() */
struct gmio_stlb_write_options
{
    /*! Header data consisting of 80 bytes
     *
     *  Defaulted to an array containing 0 when :
     *    \li calling gmio_stlb_write() with <tt>options == NULL</tt>
     *    \li OR <tt>header_data == NULL</tt>
     */
    const uint8_t* header_data;

    /*! Byte order of the output STL binary data
     *
     *  Defaulted to host's endianness when calling gmio_stlb_write()
     *  with \c options==NULL
     */
    gmio_endianness_t byte_order;
};
typedef struct gmio_stlb_write_options  gmio_stlb_write_options_t;

#endif /* GMIO_STL_IO_OPTIONS_H */
