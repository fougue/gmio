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
** "http://www.cecill.info".
****************************************************************************/

/*! \file stl_io.h
 *  STL read/write functions
 */

#ifndef GMIO_LIBSTL_STL_IO_H
#define GMIO_LIBSTL_STL_IO_H

#include "stl_global.h"
#include "stl_mesh.h"
#include "stl_mesh_creator.h"
#include "../gmio_core/endian.h"
#include "../gmio_core/transfer.h"

GMIO_C_LINKAGE_BEGIN

/* ========================================================================
 *  STL ascii
 * ======================================================================== */

/*! Options for gmio_stla_read()
 *
 *  Possible other options in the future:
 *     - flag to force locale ?
 *     - case sensitive/insensitive ?
 */
struct gmio_stla_read_options
{
    /*! Hint about the total size (in bytes) of the STL ascii data to be read
     *  from stream
     *
     *  \p stream_size is passed to gmio_transfer::handle_progress_func() as
     *  the \p max_value argument.
     *
     *  Defaulted to \c 0 when calling gmio_stla_read() with \c options==NULL
     */
    size_t stream_size;
};
typedef struct gmio_stla_read_options  gmio_stla_read_options_t;

/*! Reads geometry from STL ascii stream
 *
 *  \param mesh Defines the callbacks for the mesh creation
 *  \param trsf Defines needed objects for the read operation
 *  \param options Options for the operation, can be \c NULL to use default
 *                 values
 *
 *  \return Error code (see error.h and stl_error.h)
 */
GMIO_LIBSTL_EXPORT
int gmio_stla_read(gmio_stl_mesh_creator_t* creator,
                   gmio_transfer_t* trsf,
                   const gmio_stla_read_options_t* options);


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
    uint8_t     float32_prec;
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
 *  \retval GMIO_INVALID_BUFFER_SIZE_ERROR if \c trs->buffer_size < 512
 */
GMIO_LIBSTL_EXPORT
int gmio_stla_write(const gmio_stl_mesh_t* mesh,
                    gmio_transfer_t* trsf,
                    const gmio_stla_write_options_t* options);

/* ========================================================================
 *  STL binary
 * ======================================================================== */

/*! Options for gmio_stlb_read() */
struct gmio_stlb_read_options
{
    /*! Byte order of the input STL binary data
     *
     *  Defaulted to host's endianness when calling gmio_stlb_read()
     *  with \c options==NULL
     */
    gmio_endianness_t byte_order;
};
typedef struct gmio_stlb_read_options  gmio_stlb_read_options_t;

/*! Reads geometry from STL binary stream
 *
 *  \param mesh Defines the callbacks for the mesh creation
 *  \param trsf Defines needed objects for the read operation
 *  \param options Options for the operation, can be \c NULL to use default
 *                 values
 *
 *  \return Error code (see error.h and stl_error.h)
 *  \retval GMIO_INVALID_BUFFER_SIZE_ERRO
 *          if \c trs->buffer_size < GMIO_STLB_MIN_CONTENTS_SIZE
 */
GMIO_LIBSTL_EXPORT
int gmio_stlb_read(gmio_stl_mesh_creator_t* creator,
                   gmio_transfer_t* trsf,
                   const gmio_stlb_read_options_t* options);


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
 *  \retval GMIO_INVALID_BUFFER_SIZE_ERRO
 *          if \c trs->buffer_size < GMIO_STLB_MIN_CONTENTS_SIZE
 */
GMIO_LIBSTL_EXPORT
int gmio_stlb_write(const gmio_stl_mesh_t* mesh,
                    gmio_transfer_t* trsf,
                    const gmio_stlb_write_options_t* options);

GMIO_C_LINKAGE_END

#endif /* GMIO_LIBSTL_STL_IO_H */
