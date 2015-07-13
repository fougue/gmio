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

#ifndef GMIO_SUPPORT_GLOBAL_H
#define GMIO_SUPPORT_GLOBAL_H

#include "../gmio_core/global.h"

#ifdef GMIO_LIBSUPPORT_DLL
#  ifdef GMIO_LIBSUPPORT_MAKE_DLL
#    define GMIO_LIBSUPPORT_EXPORT GMIO_DECL_EXPORT
#  else
#    define GMIO_LIBSUPPORT_EXPORT GMIO_DECL_IMPORT
#  endif /* GMIO_LIBSUPPORT_MAKE_DLL */
#else
#  define GMIO_LIBSUPPORT_EXPORT
#endif /* GMIO_LIBSUPPORT_DLL */

#endif /* GMIO_SUPPORT_GLOBAL_H */
