#ifndef GMIO_LIBSTL_STL_IO_H
#define GMIO_LIBSTL_STL_IO_H

#include "stl_global.h"
#include "stl_mesh.h"
#include "stl_mesh_creator.h"
#include "../gmio_core/endian.h"
#include "../gmio_core/transfer.h"

GMIO_C_LINKAGE_BEGIN

/* ============================================================================
 *  STL ascii
 * ============================================================================
 */

/*! \brief Options for gmio_stla_read()
 *
 *  No options yet, it just exists for future needs
 */
struct gmio_stla_read_options
{
  void* dummy; /*!< Structs must have at least one member in strict ISO-C90 */
  /* Flag to force locale ? */
};
typedef struct gmio_stla_read_options  gmio_stla_read_options_t;

/*! \brief Read geometry from STL ascii stream
 *
 *  \p options should be always set to NULL (not used for the moment)
 */
GMIO_LIBSTL_EXPORT
int gmio_stla_read(gmio_stl_mesh_creator_t* creator,
                   gmio_transfer_t* trsf,
                   const gmio_stla_read_options_t* options); /* NULL */

/*! \brief Options for gmio_stla_write()
 */
struct gmio_stla_write_options
{
  /*! May be NULL to generate default name */
  const char* solid_name;

  /*! The maximum number of significant digits(set to 9 if options == NULL) */
  uint8_t     real32_prec;
};
typedef struct gmio_stla_write_options  gmio_stla_write_options_t;

/*! \brief Write geometry in the STL ascii format
 *
 *  \param mesh Defines the mesh to write
 *  \param trsf Defines needed objects (stream, buffer, ...) for the writing
 *              operation
 *  \param options Options for the writing operation
 *
 *  \return Error code
 *  \retval GMIO_NO_ERROR If operation successful
 */
GMIO_LIBSTL_EXPORT
int gmio_stla_write(const gmio_stl_mesh_t* mesh,
                    gmio_transfer_t* trsf,
                    const gmio_stla_write_options_t* options);

/* ============================================================================
 *  STL binary
 * ============================================================================
 */

/*! \brief Options for gmio_stlb_read()
 */
struct gmio_stlb_read_options
{
  /*! Set to host byte order if not specified (ie. options == NULL) */
  gmio_endianness_t byte_order;
};
typedef struct gmio_stlb_read_options  gmio_stlb_read_options_t;

/*! \brief Read geometry from STL binary stream
 */
GMIO_LIBSTL_EXPORT
int gmio_stlb_read(gmio_stl_mesh_creator_t* creator,
                   gmio_transfer_t* trsf,
                   const gmio_stlb_read_options_t* options);


/*! \brief Options for gmio_stlb_write()
 */
struct gmio_stlb_write_options
{
  /*! Header data consisting of 80 bytes */
  const uint8_t* header_data;

  /*! Set to host byte order if not specified (ie. options == NULL) */
  gmio_endianness_t byte_order;
};
typedef struct gmio_stlb_write_options  gmio_stlb_write_options_t;

/*! \brief Write geometry in the STL binary format
 */
GMIO_LIBSTL_EXPORT
int gmio_stlb_write(const gmio_stl_mesh_t* mesh,
                    gmio_transfer_t* trsf,
                    const gmio_stlb_write_options_t* options);

GMIO_C_LINKAGE_END

#endif /* GMIO_LIBSTL_STL_IO_H */
