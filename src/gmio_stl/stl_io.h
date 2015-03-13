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
     *  It's defaulted to \c 0 if options argument is set to NULL (when
     *  calling gmio_stla_read())
     */
    size_t stream_size;
};
typedef struct gmio_stla_read_options  gmio_stla_read_options_t;

/*! Reads geometry from STL ascii stream
 *
 *  \p options should be always set to NULL (not used for the moment)
 */
GMIO_LIBSTL_EXPORT
int gmio_stla_read(gmio_stl_mesh_creator_t* creator,
                   gmio_transfer_t* trsf,
                   const gmio_stla_read_options_t* options);

/*! Options for gmio_stla_write() */
struct gmio_stla_write_options
{
    /*! May be NULL to generate default name */
    const char* solid_name;

    /*! The maximum number of significant digits(set to 9 if options == NULL) */
    uint8_t     float32_prec;
};
typedef struct gmio_stla_write_options  gmio_stla_write_options_t;

/*! Writes geometry in the STL ascii format
 *
 *  \param mesh Defines the mesh to write
 *  \param trsf Defines needed objects (stream, buffer, ...) for the writing
 *              operation
 *  \param options Options for the writing operation, can be set to NULL to
 *                 use default values
 *
 *  \return Error code
 *  \retval GMIO_NO_ERROR If operation successful
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
    /*! Set to host byte order if not specified (ie. options == NULL) */
    gmio_endianness_t byte_order;
};
typedef struct gmio_stlb_read_options  gmio_stlb_read_options_t;

/*! Reads geometry from STL binary stream */
GMIO_LIBSTL_EXPORT
int gmio_stlb_read(gmio_stl_mesh_creator_t* creator,
                   gmio_transfer_t* trsf,
                   const gmio_stlb_read_options_t* options);


/*! Options for gmio_stlb_write() */
struct gmio_stlb_write_options
{
    /*! Header data consisting of 80 bytes */
    const uint8_t* header_data;

    /*! Set to host byte order if not specified (ie. options == NULL) */
    gmio_endianness_t byte_order;
};
typedef struct gmio_stlb_write_options  gmio_stlb_write_options_t;

/*! Writes geometry in the STL binary format */
GMIO_LIBSTL_EXPORT
int gmio_stlb_write(const gmio_stl_mesh_t* mesh,
                    gmio_transfer_t* trsf,
                    const gmio_stlb_write_options_t* options);

GMIO_C_LINKAGE_END

#endif /* GMIO_LIBSTL_STL_IO_H */
