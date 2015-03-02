/****************************************************************************
**
** GeomIO Library
** Copyright FougSys (2 Mar. 2015)
** contact@fougsys.fr
**
** This software is a reusable library whose purpose is to provide complete
** I/O support for various CAD file formats (eg. STL)
**
** This software is governed by the CeCILL-B license under French law and
** abiding by the rules of distribution of free software.  You can  use,
** modify and/ or redistribute the software under the terms of the CeCILL-B
** license as circulated by CEA, CNRS and INRIA at the following URL
** "http://www.cecill.info".
**
****************************************************************************/

#ifndef GMIO_INTERNAL_ASCII_PARSE_H
#define GMIO_INTERNAL_ASCII_PARSE_H

#include "../global.h"
#include "../stream.h"

struct gmio_ascii_string_buffer
{
  char*  ptr;       /*!< Buffer contents */
  size_t len;       /*!< Size(length) of current contents */
  size_t max_len;   /*!< Maximum contents size(length) */
};

typedef struct gmio_ascii_string_buffer  gmio_ascii_string_buffer_t;

struct gmio_ascii_stream_fwd_iterator
{
  gmio_stream_t*             stream;
  gmio_ascii_string_buffer_t buffer;
  size_t                     buffer_pos; /*!< Position indicator in buffer */

  void* cookie;
  void (*stream_read_hook)(void* cookie, const gmio_ascii_string_buffer_t* str_buffer);
};

typedef struct gmio_ascii_stream_fwd_iterator  gmio_ascii_stream_fwd_iterator_t;

void  gmio_ascii_stream_fwd_iterator_init(gmio_ascii_stream_fwd_iterator_t* it);
char* gmio_current_char(const gmio_ascii_stream_fwd_iterator_t* it);
void  gmio_skip_spaces(gmio_ascii_stream_fwd_iterator_t* it);
int   gmio_eat_word(gmio_ascii_stream_fwd_iterator_t* it, gmio_ascii_string_buffer_t* buffer);

/*! \brief Converts the string pointed to by \p str to gmio_real32_t representation
 *
 *  \retval 0  On success
 *  \retval -1 On error(check \c errno to see what happened)
 */
int   gmio_get_real32(const char* str, gmio_real32_t* value_ptr);

char*       gmio_next_char(gmio_ascii_stream_fwd_iterator_t* it);
gmio_bool_t gmio_checked_next_chars(gmio_ascii_stream_fwd_iterator_t* it, const char* str);

#endif /* GMIO_INTERNAL_ASCII_PARSE_H */
