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

/* WARNING :
 *     this header has no multi-inclusion guard. It must be included only once
 *     in the translation unit of use. The reason is that all functions
 *     defined here are meant to be inlined for performance purpose
 */

#include "../error.h"

/*! Returns true if <tt>code == GMIO_NO_ERROR</tt> */
GMIO_INLINE gmio_bool_t gmio_no_error(int code)
{
    return code == GMIO_NO_ERROR;
}

/*! Returns true if <tt>code != GMIO_NO_ERROR</tt> */
GMIO_INLINE gmio_bool_t gmio_error(int code)
{
    return code != GMIO_NO_ERROR;
}
