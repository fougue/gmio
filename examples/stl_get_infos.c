/* -----------------------------------------------------------------------------
 *
 * Example: get informations about an STL file
 *
 * Informations that can be retrieved by gmio_stl_infos_get() are:
 *   - STL format of the input stream
 *   - Count of facets(triangles)
 *   - Size of the STL contents in bytes
 *   - STL ascii only: name of the solid
 *   - STL binary only: header(80-bytes)
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
    return NULL;
}

static void print_stl_infos(const struct gmio_stl_infos* infos)
{
    printf("Format: %s\n"
           "Size: %uKo\n"
           "Facets: %u\n",
           gmio_stl_format_to_str(infos->format),
           infos->size / 1024,
           infos->facet_count);
    if (infos->format == GMIO_STL_FORMAT_ASCII)
        printf("Solid name: %s\n", infos->stla_solidname);
    else if (infos->format & GMIO_STL_FORMAT_TAG_BINARY)
        printf("Header: %80.80s\n", infos->stlb_header.data);
}

int main(int argc, char** argv)
{
    int error = 0;
    if (argc > 1) {
        const char* filepath = argv[1];
        FILE* file = fopen(filepath, "rb");

        if (file != NULL) {
            struct gmio_stream stream = gmio_stream_stdio(file);
            struct gmio_stl_infos infos = {0};
            /* Retrieve STL informations, using default options(NULL) */
            error = gmio_stl_infos_get(
                        &infos, &stream, GMIO_STL_INFO_FLAG_ALL, NULL);
            printf("File: %s\n", filepath);
            if (error == GMIO_ERROR_OK)
                print_stl_infos(&infos);
            else
                fprintf(stderr, "gmio error: 0x%X\n", error);

            fclose(file);
        }
    }
    return error;
}
