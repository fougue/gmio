/****************************************************************************
** Copyright (c) 2017, Fougue Ltd. <http://www.fougue.pro>
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
**     1. Redistributions of source code must retain the above copyright
**        notice, this list of conditions and the following disclaimer.
**
**     2. Redistributions in binary form must reproduce the above
**        copyright notice, this list of conditions and the following
**        disclaimer in the documentation and/or other materials provided
**        with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

#pragma once

#include "../gmio_core/global.h"

/*! @} */
