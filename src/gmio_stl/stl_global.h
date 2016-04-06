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

/*! \file stl_global.h
 *  Global declarations for the STL module
 *
 *  \defgroup gmio_stl gmioSTL
 *  Provides API to handle input/output operations with the STL file format
 *
 *  Support of the STL file format (STereoLithography) is complete :
 *
 *  \li ASCII format: Case-insensitive reading
 *  \li ASCII format: Output format(\%f, \%e, ...) and precision of floats support
 *  \li Binary format: Little/big endian support
 *  \li Binary format: 80-byte header and facet "attribute byte count" support
 *  \li Detection of the input format
 *  \li Retrieval of infomations about contents(facet count, solid name, ...)
 *  \li Multiple solids from stream(eg. 4 solids in STL ascii file)
 *
 *  In addition, the gmioSTL module has the following advatanges:
 *
 *  \li The user keeps its own geometry data structures, no conversion needed
 *  \li Fixed memory consumption and independant of the mesh size
 *  \li Seamless use of OpenCascade \c StlMesh_Mesh and \c MeshVS_DataSource in
 *      gmio(see \c gmioSupport module)
 *
 *  In this module, the name of all entities(structures, functions, ...) are
 *  prefixed either with :
 *  \li \c gmio_stl
 *  \li <tt>gmio_stla</tt>, this applies only for STL ascii
 *  \li <tt>gmio_stlb</tt>, this applies only for STL binary(little/big endian)
 *
 *  \addtogroup gmio_stl
 *  @{
 */

#ifndef GMIO_STL_GLOBAL_H
#define GMIO_STL_GLOBAL_H

#include "../gmio_core/global.h"

#endif /* GMIO_STL_GLOBAL_H */
/*! @} */
