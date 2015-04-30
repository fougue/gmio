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

/*! \file text_format.h
 *  Formats for textual representation
 */

#ifndef GMIO_TEXT_FORMAT_H
#define GMIO_TEXT_FORMAT_H

#include "global.h"

/*! This enum defines the various formats to textually represent a float */
enum gmio_float_text_format
{
    /*! Decimal floating point, lowercase (ex: 392.65) */
    GMIO_FLOAT_TEXT_FORMAT_DECIMAL_LOWERCASE = 0,
    /*! Decimal floating point, uppercase (ex: 392.65) */
    GMIO_FLOAT_TEXT_FORMAT_DECIMAL_UPPERCASE,
    /*! Scientific notation, lowercase (ex: 3.9265e+2) */
    GMIO_FLOAT_TEXT_FORMAT_SCIENTIFIC_LOWERCASE,
    /*! Scientific notation, uppercase (ex: 3.9265E+2) */
    GMIO_FLOAT_TEXT_FORMAT_SCIENTIFIC_UPPERCASE,
    /*! Use the shortest representation: decimal or scientific lowercase */
    GMIO_FLOAT_TEXT_FORMAT_SHORTEST_LOWERCASE,
    /*! Use the shortest representation: decimal or scientific uppercase */
    GMIO_FLOAT_TEXT_FORMAT_SHORTEST_UPPERCASE
};
typedef enum gmio_float_text_format gmio_float_text_format_t;

#endif /* GMIO_TEXT_FORMAT_H */
