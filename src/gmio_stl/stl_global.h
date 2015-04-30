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

/*! \file stl_global.h
 *  Global declarations for the STL module
 */

#ifndef GMIO_STL_GLOBAL_H
#define GMIO_STL_GLOBAL_H

#include "../gmio_core/global.h"

#ifdef GMIO_LIBSTL_DLL
#  ifdef GMIO_LIBSTL_MAKE_DLL
#    define GMIO_LIBSTL_EXPORT GMIO_DECL_EXPORT
#  else
#    define GMIO_LIBSTL_EXPORT GMIO_DECL_IMPORT
#  endif /* GMIO_DATAX_LIBSTL_MAKE_DLL */
#else
/*! Expands either to GMIO_DECL_EXPORT or GMIO_DECL_IMPORT when respectively
 *  compiling/using the DLL */
#   define GMIO_LIBSTL_EXPORT
#endif /* GMIO_LIBSTL_DLL */

/*! Constants for the STL binary format */
enum {
    /*! Size(in bytes) of the header data for STL binary */
    GMIO_STLB_HEADER_SIZE = 80,

    /*! Size(in bytes) of the minimum contents possible with the STL binary
     *  format */
    GMIO_STLB_MIN_CONTENTS_SIZE = 284
};

#endif /* GMIO_STL_GLOBAL_H */
