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

/*! \file stream_pos.h
 *  Declaration of gmio_stream_pos and utility functions
 *
 *  \addtogroup gmio_core
 *  @{
 */

#ifndef GMIO_STREAM_POS_H
#define GMIO_STREAM_POS_H

#include "global.h"

/*! Size of the byte array gmio_stream_pos::cookie */
enum { GMIO_STREAM_POS_COOKIE_SIZE = 32 /* bytes */ };

/*! Specifies a position within a stream
 *
 *  The information in gmio_stream_pos objects is usually filled by a call to
 *  gmio_stream::func_get_pos(), which takes a pointer to an object of this type
 *  as argument.
 *
 *  The content of a gmio_stream_pos object is not meant to be read directly,
 *  but only to be used as an argument in a call to gmio_stream::func_set_pos()
 */
struct gmio_stream_pos
{
    /*! Stores the actual(concrete) stream position object */
    uint8_t cookie[GMIO_STREAM_POS_COOKIE_SIZE];
};

GMIO_C_LINKAGE_BEGIN

/*! Returns a null stream position */
GMIO_LIB_EXPORT struct gmio_stream_pos gmio_stream_pos_null();

GMIO_C_LINKAGE_END

#endif /* GMIO_STREAM_POS_H */
/*! @} */
