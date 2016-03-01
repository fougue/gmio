/* -----------------------------------------------------------------------------
 *
 * Example: read a STL file
 *
 * Just give a filepath and an initialized gmio_stl_mesh_creator object to
 * gmio_stl_read_file().
 * The gmio_stl_mesh_creator object holds pointers to the callbacks invoked
 * during the read operation.
 * These callbacks creates the final mesh object.
 *
 * Note if you want to have control over the stream to be used, call
 * gmio_stl_read() instead.
 *
 * -------------------------------------------------------------------------- */

#include <gmio_core/error.h>
#include <gmio_stl/stl_infos.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

static const char* gmio_stl_format_to_str(enum gmio_stl_format format)
{
    switch (format) {
    case GMIO_STL_FORMAT_UNKNOWN: return "UNKNOWN";
    case GMIO_STL_FORMAT_ASCII: return "STL ASCII";
    case GMIO_STL_FORMAT_BINARY_LE: return "STL BINARY LITTLE-ENDIAN";
    case GMIO_STL_FORMAT_BINARY_BE: return "STL BINARY BIG-ENDIAN";
    }
}

static void print_stl_infos(const struct gmio_stl_infos* infos)
{
    printf("File: %s\n"
           "Format: %s\n"
           "Size: %uKo\n"
           "Facets: %u\n",
           filepath,
           gmio_stl_format_to_str(infos->format),
           infos->size / 1024,
           infos->facet_count);
    if (infos->format == GMIO_STL_FORMAT_ASCII) {
        printf("Solid name: %s\n", infos->stla_solidname);
    }
    else if (infos->format == GMIO_STL_FORMAT_BINARY_LE
             || infos->format == GMIO_STL_FORMAT_BINARY_BE)
    {
        printf("Header: %80.80s\n", infos->stlb_header.data);
    }
}

int main(int argc, char** argv)
{
    int error = 0;
    if (argc > 1) {
        /* Path to the STL file */
        const char* filepath = argv[1];
        /* Read-only standard stream on the STL file */
        FILE* file = fopen(filepath, "rb");

        if (file != NULL) {
            /* gmio stream interface object */
            struct gmio_stream stream = gmio_stream_stdio(file);
            /* Will holds informations about the STL file */
            struct gmio_stl_infos infos = {0};
            /* Retrieve STL informations */
            error = gmio_stl_infos_get(
                        &infos, &stream, GMIO_STL_INFO_FLAG_ALL, NULL);
            if (error == GMIO_ERROR_OK)
                print_stl_infos(&infos);
            else
                fprintf(stderr, "gmio error: 0x%X\n", error);

            fclose(file);
        }
    }
    return error;
}
