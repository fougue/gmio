/****************************************************************************
** gmio
** Copyright Fougue (24 Jun. 2016)
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
 *  Support of the STL file format(STereoLithography) is complete :
 *  \li ASCII format: Case-insensitive reading
 *  \li ASCII format: Support of output format(<tt>\%f</tt>, <tt>\%e</tt>, ...)
 *      and precision of floats
 *  \li Binary format: Support of little/big endianness
 *  \li Binary format: Support of 80-byte header and facet "attribute byte count"
 *  \li Support of multiple solids from stream(eg. 4 solids in STL ascii file)
 *  \li Detection of the input format
 *  \li Retrieval of infomations about contents
 *
 *  In addition, the gmioSTL module has the following advatanges:
 *  \li The user keeps its own geometry data structures, no conversion needed.
 *  \li Fixed memory consumption and independant of the mesh size
 *  \li Seamless use of OpenCascade in gmio(see \ref gmio_support module)
 *
 *  In this module, the name of all entities(structures, functions, ...) are
 *  prefixed either with :
 *  \li \c gmio_stl
 *  \li <tt>gmio_stla</tt>, this applies only for STL ascii
 *  \li <tt>gmio_stlb</tt>, this applies only for STL binary(little/big endian)
 *
 *  <table>
 *    <tr>
 *      <th></th>  <th>Functions</th>  <th>Structures</th>
 *    </tr>
 *    <tr>
 *      <td>Read</td>
 *      <td>gmio_stl_read()<br/>
 *          gmio_stl_read_file()<br/>
 *          gmio_stla_read()<br/>
 *          gmio_stlb_read()</td>
 *      <td>gmio_stl_read_options<br/>
 *          gmio_stl_mesh_creator<br/>
 *          gmio_stl_mesh_creator_infos<br/>
 *          gmio_stlb_header</td>
 *    </tr>
 *    <tr>
 *      <td>Write</td>
 *      <td>gmio_stl_write()<br/>
 *          gmio_stl_write_file()<br/>
 *          gmio_stlb_header_write()</td>
 *      <td>gmio_stl_write_options<br/>
 *          gmio_stl_mesh<br/>
 *          gmio_stlb_header</td>
 *    </tr>
 *    <tr>
 *      <td>Infos on contents</td>
 *      <td>gmio_stl_infos_get()<br/>
 *          gmio_stla_infos_get_streamsize()</td>
 *      <td>gmio_stl_infos<br/>
 *          gmio_stl_infos_get_options</td>
 *    </tr>
 *    <tr>
 *      <td>Detect format</td>
 *      <td>gmio_stl_format_probe()<br/>
 *          gmio_stl_format_probe_file()</td>
 *       <td></td>
 *    </tr>
 *    <tr>
 *      <td>Utilities</td>
 *      <td>gmio_stl_triangle_compute_normal()<br/>
 *          gmio_stlb_header_str()<br/>
 *          gmio_stlb_header_to_printable_str()</td>
 *      <td>gmio_stl_triangle</td>
 *    </tr>
 *  </table>
 *
 *  \addtogroup gmio_stl
 *  @{
 */

#ifndef GMIO_STL_GLOBAL_H
#define GMIO_STL_GLOBAL_H

#include "../gmio_core/global.h"

#endif /* GMIO_STL_GLOBAL_H */
/*! @} */
