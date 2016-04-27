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

/*! \file support_global.h
 *  Global declarations for the support module
 *
 *  \defgroup gmio_support gmioSupport
 *  Provides seamless integration with other 3rd-party libraries
 *
 *  \c gmioSupport is the bridge between \c gmio and other 3rd-party libraries
 *  (eg. OpenCascade, Qt, ...)\n
 *
 *  <table> <caption>OpenCascade support</caption>
 *    <tr>  <th></th> <th colspan="3">STL</th>  </tr>
 *    <tr>  <td></td> <th>import</th> <th>export</th> <th>header</th>  </tr>
 *    <tr>
 *      <td>StlMesh_Mesh</td>
 *      <td style="color:green">yes</td>
 *      <td style="color:green">yes</td>
 *      <td>stl_occ_mesh.h</td>
 *    </tr>
 *    <tr>
 *      <td>MeshVS_DataSource</td>
 *      <td style="color:red">no</td>
 *      <td style="color:green">yes</td>
 *      <td>stl_occ_meshvs.h</td>
 *    </tr>
 *    <tr>
 *      <td>TopoDS_Shape</td>
 *      <td style="color:red">no</td>
 *      <td style="color:green">yes</td>
 *      <td>stl_occ_brep.h</td>
 *    </tr>
 *  </table>
 *
 *  \n
 *
 *  <table> <caption>I/O stream support</caption>
 *    <tr>  <th></th> <th>read</th> <th>write</th> <th>header</th>  </tr>
 *    <tr>
 *      <td>Qt QIODevice</td>
 *      <td style="color:green">yes</td>
 *      <td style="color:green">yes</td>
 *      <td>stream_qt.h</td>
 *    </tr>
 *    <tr>
 *      <td>std::basic_istream<></td>
 *      <td style="color:green">yes</td>
 *      <td style="color:red">no</td>
 *      <td>stream_cpp.h</td>
 *    </tr>
 *    <tr>
 *      <td>std::basic_ostream<></td>
 *      <td style="color:red">no</td>
 *      <td style="color:green">yes</td>
 *      <td>stream_cpp.h</td>
 *    </tr>
 *  </table>
 *
 *  To avoid the dependency of \c gmio library on some other binaries,
 *  compilation of \c gmioSupport is left to the developer.\n
 *  For example if Qt streams are needed then the target project must build
 *  somehow <tt>gmio_support/stream_qt.cpp</tt>\n
 *  All \c gmio_support source files are copied with install target (ie by doing
 *  <tt>(n)make install</tt>) to <tt>PREFIX/src/gmio_support</tt>
 *
 *  \addtogroup gmio_support
 *  @{
 */

/*
 *  OpenCascade support :
 *   |                   |       STL       |
 *   |                   | import | export |
 *   |-------------------|--------|--------|
 *   | StlMesh_Mesh      |  yes   |  yes   |
 *   | MeshVS_DataSource |  no    |  yes   |
 *   | TopoDS_Shape      |  no    |  yes   |
 *
 *  I/O stream support :
 *   |                      | read | write |
 *   |----------------------|------|-------|
 *   | Qt QIODevice         |  yes |  yes  |
 *   | std::basic_istream<> |  yes |  no   |
 *   | std::basic_ostream<> |  no  |  yes  |
 *
 */

#ifndef GMIO_SUPPORT_GLOBAL_H
#define GMIO_SUPPORT_GLOBAL_H

#include "../gmio_core/global.h"

#endif /* GMIO_SUPPORT_GLOBAL_H */
/*! @} */
