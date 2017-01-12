/****************************************************************************
** Copyright (c) 2016, Fougue Ltd. <http://www.fougue.pro>
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

/*! \file amf_error.h
 *  List of errors specific to AMF I/O functions
 *
 *  \addtogroup gmio_amf
 *  @{
 */

#ifndef GMIO_AMF_ERROR_H
#define GMIO_AMF_ERROR_H

/*! \c GMIO_AMF_ERROR_TAG
 *  Byte-mask to tag(identify) AMF-specific error codes */
enum { GMIO_AMF_ERROR_TAG = 0x02000000 };

/*! Specific error codes reported by AMF read/write functions */
enum gmio_amf_error
{
    /*! The input gmio_amf_document is \c NULL */
    GMIO_AMF_ERROR_NULL_DOCUMENT = GMIO_AMF_ERROR_TAG + 0x01,

    /*! Function pointer gmio_amf_document::func_get_document_element
     *  is \c NULL */
    GMIO_AMF_ERROR_NULL_FUNC_GET_DOCUMENT_ELEMENT,

    /*! Function pointer gmio_amf_document::func_get_constellation_instance
     *  is \c NULL */
    GMIO_AMF_ERROR_NULL_FUNC_GET_CONSTELLATION_INSTANCE,

    /*! Function pointer gmio_amf_document::func_get_object_mesh is \c NULL */
    GMIO_AMF_ERROR_NULL_FUNC_GET_OBJECT_MESH,

    /*! Function pointer gmio_amf_document::func_get_object_mesh_element
     *  is \c NULL */
    GMIO_AMF_ERROR_NULL_FUNC_GET_OBJECT_MESH_ELEMENT,

    /*! Function pointer gmio_amf_document::func_get_object_mesh_volume_triangle
     *  is \c NULL */
    GMIO_AMF_ERROR_NULL_FUNC_GET_OBJECT_MESH_VOLUME_TRIANGLE,

    /*! Function pointer gmio_amf_document::func_get_document_element_metadata
     *  is \c NULL while some gmio_amf_material::metadata_count > 0 */
    GMIO_AMF_ERROR_NULL_FUNC_GET_DOCUMENT_ELEMENT_METADATA,

    /*! Function pointer gmio_amf_document::func_get_material_composite
     *  is \c NULL while some gmio_amf_material::composite_count > 0 */
    GMIO_AMF_ERROR_NULL_FUNC_GET_MATERIAL_COMPOSITE,

    /*! Function pointer gmio_amf_document::func_get_object_mesh_vertex_metadata
     *  is \c NULL while some gmio_amf_vertex::metadata_count > 0 */
    GMIO_AMF_ERROR_NULL_FUNC_GET_OBJECT_MESH_VERTEX_METADATA,

    /*! Function pointer gmio_amf_document::func_get_object_mesh_volume_metadata
     *  is \c NULL while some gmio_amf_volume::metadata_count > 0 */
    GMIO_AMF_ERROR_NULL_FUNC_GET_OBJECT_MESH_VOLUME_METADATA
};

#endif /* GMIO_AMF_ERROR_H */
/*! @} */
