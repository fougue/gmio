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
 *
 *  \addtogroup gmio_stl
 *  @{
 */

#ifndef GMIO_STLB_HEADER_H
#define GMIO_STLB_HEADER_H

#include "stl_global.h"
#include "stl_constants.h"

/*! 80-byte data at the beginning of any STL binary file */
struct gmio_stlb_header
{
    uint8_t data[GMIO_STLB_HEADER_SIZE];
};

GMIO_C_LINKAGE_BEGIN

/*! Returns a gmio_stlb_header object whose contents is a copy of \p str
 *
 *  Only the first \c GMIO_STLB_HEADER_SIZE characters are copied.
 *
 *  If the length of \p str is less than \c GMIO_STLB_HEADER_SIZE then the
 *  remaining bytes are filled with zeroes.
 */
GMIO_LIBSTL_EXPORT
struct gmio_stlb_header gmio_stlb_header_str(const char* str);

/*! Copies \p header into C string \p str
 *
 *  It replaces non-printable bytes with \p replacement char.
 *  \p str must be at least \c GMIO_STLB_HEADER_SIZE+1 long, a terminating null
 *  character ('\0') is copied at position \c GMIO_STLB_HEADER_SIZE
 */
GMIO_LIBSTL_EXPORT
void gmio_stlb_header_to_printable_str(
        const struct gmio_stlb_header* header, char* str, char replacement);

GMIO_C_LINKAGE_END

#endif /* GMIO_STLB_HEADER_H */
/*! @} */
