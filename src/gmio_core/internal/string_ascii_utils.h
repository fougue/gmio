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

#ifndef GMIO_INTERNAL_STRING_ASCII_UTILS_H
#define GMIO_INTERNAL_STRING_ASCII_UTILS_H

#include "../global.h"

#include <stddef.h>

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

/*! Returns 0 if the first \p n characreters of \p str1 and \p str2 compare
 *  equal, non-zero otherwise
 *
 *  Comparison is case-insensitive
 */
GMIO_INLINE int gmio_ascii_strincmp(
        const char* str1, const char* str2, size_t n);

/*! Returns true if \p str starts with string \p begin
 *
 *  Comparison is case-insensitive
 */
GMIO_INLINE gmio_bool_t gmio_ascii_istarts_with(
        const char* str, const char* begin);

/*! Locate substring (insensitive case string matching)
 *
 *  Behaves the same as strstr()
 */
const char* gmio_ascii_istrstr(const char *str1, const char *str2);


/*
 * -- Implementation
 */

#include <string.h>

#ifdef GMIO_STRING_ASCII_UTILS_CTYPE_H
#  include <ctype.h>
#endif

int gmio_ascii_isspace(char c)
{
#ifdef GMIO_STRING_ASCII_UTILS_CTYPE_H
    return isspace(c);
#else
    /* 0x20 : space (SPC)
     * 0x09 : horizontal tab (TAB)
     * 0x0a : newline (LF)
     * 0x0b : vertical tab (VT)
     * 0x0c : feed (FF)
     * 0x0d : carriage return (CR)
     *
     * TODO: eliminate branch
     */
    return c == 0x20 || ((uint8_t)(c - 0x09) < 5);
#endif
}

int gmio_ascii_isdigit(char c)
{
#ifdef GMIO_STRING_ASCII_UTILS_CTYPE_H
    return isdigit(c);
#else
    /* 48 <= c <= 57 */
    return (uint8_t)(c - 48) < 10;
#endif
}

int gmio_ascii_isupper(char c)
{
#ifdef GMIO_STRING_ASCII_UTILS_CTYPE_H
    return isupper(c);
#else
    /* 65 <= c <= 90; */
    return (uint8_t)(c - 65) < 26;
#endif
}

int gmio_ascii_islower(char c)
{
#ifdef GMIO_STRING_ASCII_UTILS_CTYPE_H
    return islower(c);
#else
    /* 97 <= c <= 122; */
    return (uint8_t)(c - 97) < 26;
#endif
}

char gmio_ascii_toupper(char c)
{
#ifdef GMIO_STRING_ASCII_UTILS_CTYPE_H
    return (char)toupper(c);
#else
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
#endif
}

char gmio_ascii_tolower(char c)
{
#ifdef GMIO_STRING_ASCII_UTILS_CTYPE_H
    return (char)tolower(c);
#else
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
#endif
}

gmio_bool_t gmio_ascii_char_iequals(char c1, char c2)
{
    /* TODO: eliminate branch */
    return c1 == c2 || (gmio_ascii_toupper(c1) == gmio_ascii_toupper(c2));
}

int gmio_ascii_stricmp(const char* str1, const char* str2)
{
    while (*str1 != 0 && gmio_ascii_char_iequals(*str1, *str2)) {
        ++str1;
        ++str2;
    }
    return *str1 - *str2;
}

int gmio_ascii_strincmp(const char* str1, const char* str2, size_t n)
{
    while (n > 0 && *str1 != 0 && gmio_ascii_char_iequals(*str1, *str2)) {
        ++str1;
        ++str2;
        --n;
    }
    return gmio_ascii_tolower(*str1) - gmio_ascii_tolower(*str2);
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

#endif /* GMIO_INTERNAL_STRING_ASCII_UTILS_H */
