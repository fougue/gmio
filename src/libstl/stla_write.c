#include "stl_io.h"

#include "../error.h"
#include "../internal/libstl/stl_rw_common.h"
#include "stl_error.h"

#include <stdio.h>
#include <string.h>

/*
 * |     14     ||      17       ||      17       ||     15      |  -> 73
 * facet normal  0.986544556E+00  0.986544556E+00  0.986544556E+00
 * |   11    |
 *  outer loop
 * |   10   ||      17       ||      17       ||     15      |      -> 69
 *   vertex  0.167500112E+02  0.505000112E+02  0.000000000E+00
 *   vertex  0.164599000E+02  0.505000111E+02  0.221480112E+01
 *   vertex  0.166819000E+02  0.483135112E+02  0.221480112E+01
 * |  8   |
 *  endloop
 * |  8   |
 * endfacet
 *
 * Total without EOL = 73 + 11 + 3*69 + 8 + 8 = 307
 * Total with EOL(2 chars) = 307 + 7*2 = 321
 */

#define FOUG_STLA_FACET_SIZE          321
#define FOUG_STLA_FACET_SIZE_P2       512
#define FOUG_STLA_SOLID_NAME_MAX_LEN  512

static char* foug_write_string(char* buffer, const char* str)
{
  const char* safe_str = str != NULL ? str : "";
  strcpy(buffer, safe_str);
  return buffer + strlen(safe_str);
}

static char* foug_write_string_eol(char* buffer, const char* str)
{
  const char* safe_str = str != NULL ? str : "";
  const size_t len = strlen(safe_str);
  strncpy(buffer, safe_str, len);
  buffer[len] = '\n';
  return buffer + len + 1;
}

static char* foug_write_eol(char* buffer)
{
  *buffer = '\n';
  return buffer + 1;
}

/*static char* foug_write_space(char* buffer)
{
  *buffer = ' ';
  return buffer + 1;
}*/

static char* foug_write_nspaces(char* buffer, int n)
{
  const int offset = n;
  while (n > 0)
    *(buffer + (--n)) = ' ';
  return buffer + offset;
}

static char* foug_write_stdio_format(char* buffer, uint8_t prec)
{
  int prec_len = 0;

  buffer[0] = '%';
  buffer[1] = '.';
  prec_len = sprintf(buffer + 2, "%i", prec);
  buffer[2 + prec_len] = 'E';
  return buffer + 3 + prec_len;
}

/*static char* foug_write_real32_string(char* buffer, const char* format, foug_real32_t val)
{
  return buffer + sprintf(buffer, format, val);
}*/

static char* foug_write_coords(char* buffer,
                               const char* coords_format,
                               const foug_stl_coords_t* coords)
{
  return buffer + sprintf(buffer, coords_format, coords->x, coords->y, coords->z);
}

static foug_bool_t foug_transfer_flush_buffer(foug_transfer_t* trsf, size_t n)
{
  return foug_stream_write(&trsf->stream, trsf->buffer, sizeof(char), n) == n;
}

#define _FOUG_INTERNAL_MIN(v1, v2)  ((v1) < (v2) ? (v1) : (v2))

/*! \fn int foug_stla_write(foug_stl_geom_t*, foug_transfer_t*, const char*, uint8_t)
 *
 *  \param geom Defines the custom geometry to write
 *  \param trsf Defines needed objects (stream, buffer, ...) for the writing operation
*   \param solid_name May be NULL to generate default name
 *  \param real32_prec The maximum number of significant digits
 *
 *  \return Error code
 *
 *  \retval FOUG_DATAX_NO_ERROR If operation successful
 */
int foug_stla_write(foug_stl_geom_t* geom,
                    foug_transfer_t* trsf,
                    const char* solid_name,
                    uint8_t real32_prec)
{
  const uint32_t total_facet_count = geom != NULL ? geom->triangle_count : 0;
  uint32_t written_facet_count = 0;
  const uint32_t buffer_facet_count = trsf != NULL ? trsf->buffer_size / FOUG_STLA_FACET_SIZE_P2 : 0;
  uint32_t ifacet = 0;
  char* buffer_iterator = trsf != NULL ? trsf->buffer : NULL;
  char coords_format[64];
  int error = FOUG_DATAX_NO_ERROR;

  /* Check validity of input parameters */
  foug_check_transfer(&error, trsf);
  foug_stl_check_geom(&error, geom);
  if (real32_prec == 0 || real32_prec > 9)
    error = FOUG_STLA_WRITE_INVALID_REAL32_PREC_ERROR;
  if (trsf->buffer_size < FOUG_STLA_FACET_SIZE_P2)
    error = FOUG_DATAX_INVALID_BUFFER_SIZE_ERROR;
  if (foug_datax_error(error))
    return error;

  { /* Create XYZ coords format string (for normal and vertex coords) */
    char* coords_format_iterator = coords_format;
    coords_format_iterator = foug_write_stdio_format(coords_format_iterator, real32_prec);
    coords_format_iterator = foug_write_nspaces(coords_format_iterator, 2);
    coords_format_iterator = foug_write_stdio_format(coords_format_iterator, real32_prec);
    coords_format_iterator = foug_write_nspaces(coords_format_iterator, 2);
    coords_format_iterator = foug_write_stdio_format(coords_format_iterator, real32_prec);
    *coords_format_iterator = 0; /* Write terminating null byte */
    /* TODO: check the "format" string can contain the given precision */
  }

  /* Write solid declaration */
  {
    buffer_iterator = foug_write_string(buffer_iterator, "solid ");
    buffer_iterator = foug_write_string_eol(buffer_iterator, solid_name);
    if (!foug_transfer_flush_buffer(trsf, buffer_iterator - (char*)trsf->buffer))
      return FOUG_DATAX_STREAM_ERROR;
  }

  /* Write solid's facets */
  for (ifacet = 0;
       ifacet < total_facet_count && foug_datax_no_error(error);
       ifacet += buffer_facet_count)
  {
    const uint32_t clamped_facet_count = _FOUG_INTERNAL_MIN(ifacet + buffer_facet_count,
                                                            total_facet_count);
    foug_stl_triangle_t tri;
    uint32_t ibuffer_facet;

    /* Writing of facets is buffered */
    buffer_iterator = trsf->buffer;
    for (ibuffer_facet = ifacet; ibuffer_facet < clamped_facet_count; ++ibuffer_facet) {
      geom->get_triangle_func(geom->cookie, ibuffer_facet, &tri);
      buffer_iterator = foug_write_string(buffer_iterator, "facet normal  ");
      buffer_iterator = foug_write_coords(buffer_iterator, coords_format, &tri.normal);
      buffer_iterator = foug_write_eol(buffer_iterator);

      buffer_iterator = foug_write_string_eol(buffer_iterator, " outer loop");
      buffer_iterator = foug_write_string(buffer_iterator, "  vertex  ");
      buffer_iterator = foug_write_coords(buffer_iterator, coords_format, &tri.v1);
      buffer_iterator = foug_write_eol(buffer_iterator);
      buffer_iterator = foug_write_string(buffer_iterator, "  vertex  ");
      buffer_iterator = foug_write_coords(buffer_iterator, coords_format, &tri.v2);
      buffer_iterator = foug_write_eol(buffer_iterator);
      buffer_iterator = foug_write_string(buffer_iterator, "  vertex  ");
      buffer_iterator = foug_write_coords(buffer_iterator, coords_format, &tri.v3);
      buffer_iterator = foug_write_eol(buffer_iterator);
      buffer_iterator = foug_write_string_eol(buffer_iterator, " endloop");

      buffer_iterator = foug_write_string_eol(buffer_iterator, "endfacet");
    } /* end for (ibuffer_facet) */

    if (!foug_transfer_flush_buffer(trsf, buffer_iterator - (char*)trsf->buffer))
      error = FOUG_DATAX_STREAM_ERROR;

    /* Task control */
    if (foug_datax_no_error(error) && trsf->task_control.handle_progress_func != NULL) {
      uint32_t percentage = 0;
      written_facet_count += buffer_facet_count;
      percentage = foug_percentage(0, total_facet_count, written_facet_count);
      if (!foug_task_control_handle_progress(&trsf->task_control, percentage))
        error = FOUG_DATAX_TASK_STOPPED_ERROR;
    }
  } /* end for (ifacet) */

  /* Write end of solid */
  if (foug_datax_no_error(error)) {
    buffer_iterator = foug_write_string(trsf->buffer, "endsolid ");
    buffer_iterator = foug_write_string_eol(buffer_iterator, solid_name);
    if (!foug_transfer_flush_buffer(trsf, buffer_iterator - (char*)trsf->buffer))
      error = FOUG_DATAX_STREAM_ERROR;
  }

  return error;
}
