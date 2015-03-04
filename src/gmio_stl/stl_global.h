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
** "http://www.cecill.info".
****************************************************************************/

/*! \file stl_global.h
 *  Global declarations for the STL module
 */

#ifndef GMIO_LIBSTL_GLOBAL_H
#define GMIO_LIBSTL_GLOBAL_H

#include "../gmio_core/global.h"

#ifdef GMIO_LIBSTL_DLL
# ifdef GMIO_LIBSTL_MAKE_DLL
#  define GMIO_LIBSTL_EXPORT GMIO_DECL_EXPORT
# else
#  define GMIO_LIBSTL_EXPORT GMIO_DECL_IMPORT
# endif /* GMIO_DATAX_LIBSTL_MAKE_DLL */
#else
# define GMIO_LIBSTL_EXPORT
#endif /* GMIO_LIBSTL_DLL */

/* STLB */
enum { GMIO_STLB_HEADER_SIZE = 80 };
enum { GMIO_STLB_MIN_CONTENTS_SIZE = 284 };

#endif /* GMIO_LIBSTL_GLOBAL_H */
