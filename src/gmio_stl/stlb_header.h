/****************************************************************************
** gmio
** Copyright Fougue (2 Mar. 2015)
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

/*! \file stlb_header.h
 *  Declaration of gmio_stlb_header
 */

#ifndef GMIO_STLB_HEADER_H
#define GMIO_STLB_HEADER_H

#include "stl_global.h"

/*! Size(in bytes) of the header data for STL binary */
enum { GMIO_STLB_HEADER_SIZE = 80 };

/*! 80-byte data at the beginning of any STL binary file */
struct gmio_stlb_header
{
    uint8_t data[GMIO_STLB_HEADER_SIZE];
};
typedef struct gmio_stlb_header gmio_stlb_header_t;

#endif /* GMIO_STLB_HEADER_H */
