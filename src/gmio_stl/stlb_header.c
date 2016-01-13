/****************************************************************************
** gmio
** Copyright Fougue (2 Mar. 2015)
** contact@fougue.pro
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

#include "stlb_header.h"

#include <ctype.h>
#include <string.h>

struct gmio_stlb_header gmio_stlb_header_str(const char* str)
{
    struct gmio_stlb_header header = {0};
    if (str != NULL)
        strncpy((char*)header.data, str, GMIO_STLB_HEADER_SIZE);
    return header;
}

void gmio_stlb_header_to_printable_str(
        const struct gmio_stlb_header* header, char* str, char replacement)
{
    size_t i;
    for (i = 0; i < GMIO_STLB_HEADER_SIZE; ++i) {
        const int header_char = (int)header->data[i];
        str[i] = isprint(header_char) ? (char)header_char : replacement;
    }
    str[GMIO_STLB_HEADER_SIZE] = 0;
}
