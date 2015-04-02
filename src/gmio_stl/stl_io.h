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
** "http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html".
****************************************************************************/

/*! \file stl_io.h
 *  STL read/write functions
 */

#ifndef GMIO_LIBSTL_STL_IO_H
#define GMIO_LIBSTL_STL_IO_H

#include "stl_global.h"
#include "stl_mesh.h"
#include "stl_mesh_creator.h"
#include "../gmio_core/buffer.h"
#include "../gmio_core/endian.h"
#include "../gmio_core/transfer.h"

GMIO_C_LINKAGE_BEGIN

/*! Reads STL file, format is automatically guessed
 *
 *  \param filepath Path to the STL file. A stream is opened with fopen() so
 *         the string has to be encoded using the system's charset (locale-8bit)
 *  \param creator Defines the callbacks for the mesh creation
 *  \param buffer The memory block used by stream operations
 *
 *  \return Error code (see error.h and stl_error.h)
 */
GMIO_LIBSTL_EXPORT
int gmio_stl_read_file(
        const char* filepath,
        gmio_buffer_t* buffer,
        gmio_stl_mesh_creator_t* creator);

/*! Reads STL data from stream, format is automatically guessed
 *
 *  \param trsf Defines needed objects for the read operation
 *  \param creator Defines the callbacks for the mesh creation
 *
 *  \return Error code (see error.h and stl_error.h)
 */
GMIO_LIBSTL_EXPORT
int gmio_stl_read(
        gmio_transfer_t* trsf,
        gmio_stl_mesh_creator_t* creator);

/* ========================================================================
 *  STL ascii
 * ======================================================================== */

/*! Reads geometry from STL ascii stream
 *
 *  \param trsf Defines needed objects for the read operation
 *  \param creator Defines the callbacks for the mesh creation
 *
 *  Stream size is passed to gmio_transfer::handle_progress_func() as the
 *  \p max_value argument.
 *
 *  Possible options in a future version could be:
 *     - flag to force locale ?
 *     - case sensitive/insensitive ?
 *
 *  \return Error code (see error.h and stl_error.h)
 */
GMIO_LIBSTL_EXPORT
int gmio_stla_read(
        gmio_transfer_t* trsf,
        gmio_stl_mesh_creator_t* creator);


/*! Options for gmio_stla_write() */
struct gmio_stla_write_options
{
    /*! Name of the solid to appear in "solid <name> \n facet normal ..."
     *
     * Defaulted to an empty string "" when :
     *    \li calling gmio_stla_write() with <tt>options == NULL</tt>
     *    \li OR <tt>solid_name == NULL</tt>
     */
    const char* solid_name;

    /*! The maximum number of significant digits to write float values
     *
     *  Defaulted to \c 9 when calling gmio_stla_write() with \c options==NULL
     */
    uint8_t float32_prec;
};
typedef struct gmio_stla_write_options  gmio_stla_write_options_t;

/*! Writes geometry in the STL ascii format
 *
 *  \param mesh Defines the mesh to write
 *  \param trsf Defines needed objects for the write operation
 *  \param options Options for the operation, can be \c NULL to use default
 *                 values
 *
 *  \return Error code (see error.h and stl_error.h)
 *  \retval GMIO_ERROR_INVALID_BUFFER_SIZE
 *          if <tt>trsf->buffer.size < 512</tt>
 */
GMIO_LIBSTL_EXPORT
int gmio_stla_write(
        gmio_transfer_t* trsf,
        const gmio_stl_mesh_t* mesh,
        const gmio_stla_write_options_t* options);

/* ========================================================================
 *  STL binary
 * ======================================================================== */

/*! Reads geometry from STL binary stream
 *
 *  \param trsf Defines needed objects for the read operation
 *  \param creator Defines the callbacks for the mesh creation
 *  \param byte_order Byte order of the input STL binary data
 *
 *  \return Error code (see error.h and stl_error.h)
 *  \retval GMIO_ERROR_INVALID_BUFFER_SIZE
 *          if <tt>trsf->buffer.size < GMIO_STLB_MIN_CONTENTS_SIZE</tt>
 */
GMIO_LIBSTL_EXPORT
int gmio_stlb_read(
        gmio_transfer_t* trsf,
        gmio_stl_mesh_creator_t* creator,
        gmio_endianness_t byte_order);


/*! Options for gmio_stlb_write() */
struct gmio_stlb_write_options
{
    /*! Header data consisting of 80 bytes
     *
     *  Defaulted to an array containing 0 when :
     *    \li calling gmio_stlb_write() with <tt>options == NULL</tt>
     *    \li OR <tt>header_data == NULL</tt>
     */
    const uint8_t* header_data;

    /*! Byte order of the output STL binary data
     *
     *  Defaulted to host's endianness when calling gmio_stlb_write()
     *  with \c options==NULL
     */
    gmio_endianness_t byte_order;
};
typedef struct gmio_stlb_write_options  gmio_stlb_write_options_t;

/*! Writes geometry in the STL binary format
 *
 *  \param mesh Defines the mesh to write
 *  \param trsf Defines needed objects for the write operation
 *  \param options Options for the operation, can be \c NULL to use default
 *                 values
 *
 *  \return Error code (see error.h and stl_error.h)
 *  \retval GMIO_INVALID_BUFFER_SIZE_ERROR
 *          if <tt>trsf->buffer.size < GMIO_STLB_MIN_CONTENTS_SIZE</tt>
 */
GMIO_LIBSTL_EXPORT
int gmio_stlb_write(
        gmio_transfer_t* trsf,
        const gmio_stl_mesh_t* mesh,
        const gmio_stlb_write_options_t* options);

GMIO_C_LINKAGE_END

#endif /* GMIO_LIBSTL_STL_IO_H */
