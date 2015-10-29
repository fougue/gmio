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

#ifndef GMIO_INTERNAL_STRING_UTILS_H
#define GMIO_INTERNAL_STRING_UTILS_H

#include "../global.h"

/*! Returns non-zero if \p c is a space, zero otherwise */
GMIO_INLINE int gmio_ascii_isspace(char c);

/*! Returns non-zero if \p c is a digit, zero otherwise */
GMIO_INLINE int gmio_ascii_isdigit(char c);

/*! Returns non-zero if \p c is an uppercase letter, zero otherwise */
GMIO_INLINE int gmio_ascii_isupper(char c);

/*! Returns non-zero if \p c is a lowercase letter, zero otherwise */
GMIO_INLINE int gmio_ascii_islower(char c);

/*! Returns the lowercase letter converted to uppercase */
GMIO_INLINE char gmio_ascii_toupper(char c);

/*! Returns the uppercase letter converted to lowercase */
GMIO_INLINE char gmio_ascii_tolower(char c);

/*! Returns 0 if \p c1 compare equals to \p c2, non-zero otherwise
 *
 *  Comparison is case-insensitive
 */
GMIO_INLINE gmio_bool_t gmio_ascii_char_iequals(char c1, char c2);

/*! Returns 0 if \p str1 and \p str2 compare equal, non-zero otherwise
 *
 *  Comparison is case-insensitive
 */
GMIO_INLINE int gmio_ascii_stricmp(const char* str1, const char* str2);

/*! Returns true if \p str starts with string \p begin
 *
 *  Comparison is case-insensitive
 */
GMIO_INLINE gmio_bool_t gmio_ascii_istarts_with(
        const char* str, const char* begin);



/*
 * -- Implementation
 */

#include <string.h>

#ifdef GMIO_STRINGUTILS_CTYPE_H
#  include <ctype.h>
#endif

int gmio_ascii_isspace(char c)
{
    /* 0x20 : space (SPC)
     * 0x09 : horizontal tab (TAB)
     * 0x0a : newline (LF)
     * 0x0b : vertical tab (VT)
     * 0x0c : feed (FF)
     * 0x0d : carriage return (CR) */
#if defined(GMIO_STRINGUTILS_DIRECT_TESTS)
    /* TODO: eliminate branch */
    return c == 0x20 || ((uint8_t)(c - 0x09) < 5);
#elif defined(GMIO_STRINGUTILS_CTYPE_H)
    return isspace(c);
#endif
}

int gmio_ascii_isdigit(char c)
{
    /* 48 <= c <= 57 */
#if defined(GMIO_STRINGUTILS_DIRECT_TESTS)
    return (uint8_t) (c - 48) < 10;
#elif defined(GMIO_STRINGUTILS_CTYPE_H)
    return isdigit(c);
#endif
}

int gmio_ascii_isupper(char c)
{
    /* 65 <= c <= 90; */
#if defined(GMIO_STRINGUTILS_DIRECT_TESTS)
    return (uint8_t) (c - 65) < 26;
#elif defined(GMIO_STRINGUTILS_CTYPE_H)
    return isupper(c);
#endif
}

int gmio_ascii_islower(char c)
{
    /* 97 <= c <= 122; */
#if defined(GMIO_STRINGUTILS_DIRECT_TESTS)
    return (unsigned) (c - 97) < 26;
#elif defined(GMIO_STRINGUTILS_CTYPE_H)
    return islower(c);
#endif
}

char gmio_ascii_toupper(char c)
{
    static const char table_toupper[128] = {
        0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,0x09,0x0A,  0 ,  0 ,0x0D,  0 ,  0 ,
        0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
        ' ', '!', '"', '#', '$', '%', '&','\'', '(', ')', '*', '+', ',', '-', '.', '/',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
        '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
        'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[','\\', ']', '^', '_',
        '`', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
        'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '{', '|', '}', '~', 0,
    };
    return table_toupper[(unsigned char)c];
}

char gmio_ascii_tolower(char c)
{
    static const char table_tolower[128] = {
        0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,0x09,0x0A,  0 ,  0 ,0x0D,  0 ,  0 ,
        0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
        ' ', '!', '"', '#', '$', '%', '&','\'', '(', ')', '*', '+', ',', '-', '.', '/',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
        '@', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
        'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '[','\\', ']', '^', '_',
        '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
        'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~', 0,
    };
    return table_tolower[(unsigned char)c];
}

gmio_bool_t gmio_ascii_char_iequals(char c1, char c2)
{
    /* TODO: eliminate branch */
    return c1 == c2 || (gmio_ascii_toupper(c1) == gmio_ascii_toupper(c2));
}

int gmio_ascii_stricmp(const char* str1, const char* str2)
{
    while (*str1 != 0 && *str2 != 0) {
        if (!gmio_ascii_char_iequals(*str1, *str2))
            return 1;
        ++str1;
        ++str2;
    }
    return *str1 == 0 && *str2 == 0 ? 0 : 1;
}

gmio_bool_t gmio_ascii_istarts_with(const char* str, const char* begin)
{
    while (*begin != 0) {
        if (*str == 0 || !gmio_ascii_char_iequals(*str, *begin))
            return GMIO_FALSE;
        ++str;
        ++begin;
    }
    return GMIO_TRUE;
}

#endif /* GMIO_INTERNAL_STRING_UTILS_H */
