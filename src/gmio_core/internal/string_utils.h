/****************************************************************************
** GeomIO Library
** Copyright Fougue (2 Mar. 2015)
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

#ifndef GMIO_INTERNAL_STRING_UTILS_H
#define GMIO_INTERNAL_STRING_UTILS_H

#include "../global.h"

/*! Returns non-zero if \p c is a space (for C-locale), zero otherwise */
GMIO_INLINE int gmio_clocale_isspace(char c)
{
    return c == 0x20      /* space (SPC) */
            || c == 0x09  /* horizontal tab (TAB) */
            || c == 0x0a  /* newline (LF) */
            || c == 0x0b  /* vertical tab (VT) */
            || c == 0x0c  /* feed (FF) */
            || c == 0x0d  /* carriage return (CR) */
            ;
}

/*! Returns true if \p c1 compare equals to \p c2
 *
 *  Comparison is case-insensitive
 */
GMIO_INLINE gmio_bool_t gmio_clocale_char_iequals(char c1, char c2)
{
    return c1 == c2 || c1 == (c2 - 32) ? GMIO_TRUE : GMIO_FALSE;
}

/*! Returns 0 if \p str1 and \p str2 compare equal, non-zero otherwise
 *
 *  Comparison is case-insensitive
 */
GMIO_INLINE int gmio_stricmp(const char* str1, const char* str2)
{
    while (*str1 != 0 && *str2 != 0) {
        if (gmio_clocale_char_iequals(*str1, *str2) == GMIO_FALSE)
            return 1;
        ++str1;
        ++str2;
    }
    return *str1 == 0 && *str2 == 0 ? 0 : 1;
}

/*! Returns true if \p str starts with string \p begin
 *
 *  Comparison is case-insensitive
 */
GMIO_INLINE gmio_bool_t gmio_istarts_with(const char* str, const char* begin)
{
    while (*begin != 0) {
        if (*str == 0
                || gmio_clocale_char_iequals(*str, *begin) == GMIO_FALSE)
        {
            return GMIO_FALSE;
        }
        ++str;
        ++begin;
    }
    return GMIO_TRUE;
}

#endif /* GMIO_INTERNAL_STRING_UTILS_H */
