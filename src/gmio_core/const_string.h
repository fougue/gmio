/****************************************************************************
** Copyright (c) 2016, Fougue Ltd. <http://www.fougue.pro>
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
**     1. Redistributions of source code must retain the above copyright
**        notice, this list of conditions and the following disclaimer.
**
**     2. Redistributions in binary form must reproduce the above
**        copyright notice, this list of conditions and the following
**        disclaimer in the documentation and/or other materials provided
**        with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
****************************************************************************/

/*! \file const_string.h
 *  Declaration of gmio_const_string and utility functions
 *
 *  \addtogroup gmio_core
 *  @{
 */

#ifndef GMIO_CONST_STRING_H
#define GMIO_CONST_STRING_H

#include "global.h"

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

/*! Expands to bracket initialization of a gmio_const_string from const char[]
 *
 *  Example:
 *  \code{.c}
 *      static const char token[] = "woops";
 *      struct gmio_const_string token_s = GMIO_CONST_STRING_FROM_ARRAY(token);
 *  \endcode
 */
#define GMIO_CONST_STRING_FROM_ARRAY(array) { &(array)[0], sizeof(array) - 1 }

/*! Returns an initialized gmio_const_string object */
GMIO_INLINE struct gmio_const_string gmio_const_string(const char* ptr, size_t len);

/*! Returns \c true if \p str has no characters, otherwise returns \c false */
GMIO_INLINE bool gmio_const_string_is_empty(const struct gmio_const_string* str);

/*
 * -- Implementation
 */

struct gmio_const_string gmio_const_string(const char* ptr, size_t len)
{
    struct gmio_const_string cstr;
    cstr.ptr = ptr;
    cstr.len = len;
    return cstr;
}

bool gmio_const_string_is_empty(const struct gmio_const_string* str)
{
    return str->ptr == NULL || str->len == 0;
}

#endif /* GMIO_CONST_STRING_H */
/*! @} */
