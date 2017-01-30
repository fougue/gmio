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

/*! \file amf_global.h
 *  Global declarations for the AMF module
 *
 *  \defgroup gmio_amf gmioAMF
 *  Provides API to handle input/output operations with the AMF file format
 *
 *  In addition, the gmioAMF module has the following advatanges:
 *  \li The user keeps its own geometry data structures, no conversion needed.
 *  \li Fixed memory consumption and independant of the geometry size
 *
 *  In this module, the name of all entities(structures, functions, ...) are
 *  prefixed either with \c gmio_amf
 *
 *  <table>
 *    <tr>
 *      <th></th>  <th>Functions</th>  <th>Structures</th>
 *    </tr>
 *    <tr>
 *      <td>Write</td>
 *      <td>gmio_amf_write()<br/>
 *          gmio_amf_write_file()</td>
 *      <td>gmio_amf_document <br/>
 *          gmio_amf_write_options</td>
 *    </tr>
 *  </table>
 *
 *  \addtogroup gmio_amf
 *  @{
 */

#pragma once

#include "../gmio_core/global.h"

/*! @} */
