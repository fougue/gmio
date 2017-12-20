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
 *  prefixed with \c STL_
 *
 *  <table>
 *    <tr>
 *      <th></th>  <th>Functions</th>  <th>Structures</th>
 *    </tr>
 *    <tr>
 *      <td>Read</td>
 *      <td>STL_read()<br/>
 *          STL_readAscii()<br/>
 *          STL_readBinary()</td>
 *      <td>STL_ReadOptions<br/>
 *          STL_MeshCreator<br/>
 *          STL_MeshCreatorInfos<br/>
 *          STL_BinaryHeader</td>
 *    </tr>
 *    <tr>
 *      <td>Write</td>
 *      <td>STL_write()<br/>
 *          STL_writeBinaryHeader()</td>
 *      <td>STL_WriteOptions<br/>
 *          STL_Mesh<br/>
 *          STL_BinaryHeader</td>
 *    </tr>
 *    <tr>
 *      <td>Infos on contents</td>
 *      <td>STL_probeInfos()<br/>
 *          STL_probeAsciiSolidSize()</td>
 *      <td>STL_Infos<br/>
 *          STL_ProbeInfosOptions</td>
 *    </tr>
 *    <tr>
 *      <td>Detect format</td>
 *      <td>STL_probeFormat()</td>
 *      <td></td>
 *    </tr>
 *    <tr>
 *      <td>Utilities</td>
 *      <td>STL_triangleNormal()<br/>
 *          STL_BinaryHeader::toPrintableStr()<br/>
 *          STL_BinaryHeader::fromStr()</td>
 *      <td>STL_triangle</td>
 *    </tr>
 *  </table>
 */

#pragma once

#include "../gmio_core/global.h"
