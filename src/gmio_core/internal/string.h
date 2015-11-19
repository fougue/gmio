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

#ifndef GMIO_INTERNAL_STRING_H
#define GMIO_INTERNAL_STRING_H

#include "../global.h"

#include <stddef.h>

/*! Stores a read-only string of 8-bit chars
 *
 *  For faster lookups, it knowns the length of its contents.
 */
struct gmio_const_string
{
    const char* ptr; /*!< Contents */
    size_t len;      /*!< Size(length) of current contents */
};
typedef struct gmio_const_string gmio_const_string_t;

/*! Stores a mutable string of 8-bit chars
 *
 *  For faster lookups, it knowns the length of its contents. Length must not
 *  exceeds the maximum size(capacity).
 */
struct gmio_string
{
    char*  ptr; /*!< Contents */
    size_t len; /*!< Size(length) of current contents */
    size_t max_len; /*!< Maximum contents size(capacity) */
};
typedef struct gmio_string gmio_string_t;

/*! Expands to bracket initialization of a gmio_const_string from const char[]
 *
 *  Example:
 *  \code
 *      const char token[] = "woops";
 *      gmio_const_string_t token_s = GMIO_CONST_STRING_FROM_ARRAY(token);
 *  \endcode
 */
#define GMIO_CONST_STRING_FROM_ARRAY(array) { &(array)[0], sizeof(array) - 1 }

/*! Returns an initialized gmio_const_string_t object */
GMIO_INLINE gmio_const_string_t gmio_const_string(const char* ptr, size_t len);

/*! Clears the contents of the string \p str and makes it null */
GMIO_INLINE void gmio_string_clear(gmio_string_t* str);



/*
 * -- Implementation
 */

gmio_const_string_t gmio_const_string(const char* ptr, size_t len)
{
    gmio_const_string_t cstr;
    cstr.ptr = ptr;
    cstr.len = len;
    return cstr;
}

void gmio_string_clear(gmio_string_t* str)
{
    str->ptr[0] = 0;
    str->len = 0;
}

#endif /* GMIO_INTERNAL_STRING_H */
