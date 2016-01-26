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

#include "stream.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(GMIO_HAVE_SYS_TYPES_H) && defined(GMIO_HAVE_SYS_STAT_H)
#  include <sys/types.h>
#  include <sys/stat.h>

/* gmio_stat_t: type alias on the stat structure
 * GMIO_FSTAT_FUNC_NAME: alias on the fstat() function
 */
#  if defined(GMIO_HAVE_WIN__FSTAT64_FUNC)
typedef struct __stat64 gmio_stat_t;
#    define GMIO_FSTAT_FUNC_NAME _fstat64
#  elif defined(GMIO_HAVE_POSIX_FSTAT64_FUNC)
typedef struct stat64 gmio_stat_t;
#    define GMIO_FSTAT_FUNC_NAME fstat64
#  else
typedef struct stat gmio_stat_t;
#    define GMIO_FSTAT_FUNC_NAME fstat
#  endif

/* Define wrapper around the fstat() function */
GMIO_INLINE int gmio_fstat(int fd, gmio_stat_t* buf)
{
    return GMIO_FSTAT_FUNC_NAME(fd, buf);
}

#endif /* GMIO_HAVE_SYS_TYPES_H && GMIO_HAVE_SYS_STAT_H */

struct gmio_stream gmio_stream_null()
{
    struct gmio_stream null_stream = {0};
    return null_stream;
}

static bool gmio_stream_stdio_at_end(void* cookie)
{
    return feof((FILE*)cookie);
}

static int gmio_stream_stdio_error(void* cookie)
{
    return ferror((FILE*)cookie);
}

static size_t gmio_stream_stdio_read(
        void* cookie, void* ptr, size_t item_size, size_t item_count)
{
    return fread(ptr, item_size, item_count, (FILE*)cookie);
}

static size_t gmio_stream_stdio_write(
        void* cookie, const void* ptr, size_t item_size, size_t item_count)
{
    return fwrite(ptr, item_size, item_count, (FILE*)cookie);
}

static gmio_streamsize_t gmio_stream_stdio_size(void* cookie)
{
    FILE* file = (FILE*)cookie;

#if defined(GMIO_HAVE_SYS_TYPES_H) \
    && defined(GMIO_HAVE_SYS_STAT_H) \
    && defined(GMIO_HAVE_POSIX_FILENO_FUNC)

    const int fd = fileno(file);
    if (fd != -1) {
        gmio_stat_t buf;
        if (gmio_fstat(fd, &buf) == 0)
            return buf.st_size;
    }
    return 0;

#else

    /* Excerpted from: https://www.securecoding.cert.org
     *                 item FIO19-C
     *
     * Subclause 7.21.9.2 of the C Standard [ISO/IEC 9899:2011] specifies the
     * following behavior for fseek() when opening a binary file in binary mode:
     *     A binary stream need not meaningfully support fseek calls with a
     *     whence value of SEEK_END.
     *
     * In addition, footnote 268 of subclause 7.21.3 says:
     *     Setting the file position indicator to end-of-file, as with
     *     fseek(file, 0, SEEK_END), has undefined behavior for a binary stream
     *     (because of possible trailing null characters) or for any stream with
     *     state-dependent encoding that does not assuredly end in the initial
     *     shift state.
     *
     * Subclause 7.21.9.4 of the C Standard [ISO/IEC 9899:2011] specifies the
     * following behavior for ftell() when opening a text file in text mode:
     *     For a text stream, its file position indicator contains unspecified
     *     information, usable by the fseek function for returning the file
     *     position indicator for the stream to its position at the time of the
     *     ftell call.
     * Consequently, the return value of ftell() for streams opened in text mode
     * should never be used for offset calculations other than in calls to
     * fseek().
     */

    long size = 0L;
    const long old_pos = ftell(file);

    fseek(file, 0L, SEEK_END);
    size = ftell(file);
    fseek(file, old_pos, SEEK_SET);

    return size;

#endif
}

static int gmio_stream_stdio_get_pos(void* cookie, struct gmio_streampos* pos)
{
    fpos_t fpos;
    int res = fgetpos((FILE*)cookie, &fpos);
    memcpy(&pos->cookie[0], &fpos, sizeof(fpos_t));
    return res;
}

static int gmio_stream_stdio_set_pos(void* cookie, const struct gmio_streampos* pos)
{
    fpos_t fpos;
    memcpy(&fpos, &pos->cookie[0], sizeof(fpos_t));
    return fsetpos((FILE*)cookie, &fpos);
}

struct gmio_stream gmio_stream_stdio(FILE* file)
{
    struct gmio_stream stream = gmio_stream_null();
    stream.cookie = file;
    stream.func_at_end = gmio_stream_stdio_at_end;
    stream.func_error = gmio_stream_stdio_error;
    stream.func_read = gmio_stream_stdio_read;
    stream.func_write = gmio_stream_stdio_write;
    stream.func_size = gmio_stream_stdio_size;
    stream.func_get_pos = gmio_stream_stdio_get_pos;
    stream.func_set_pos = gmio_stream_stdio_set_pos;
    return stream;
}
