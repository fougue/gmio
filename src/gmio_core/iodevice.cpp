/****************************************************************************
** Copyright (c) 2017, Fougue Ltd. <http://www.fougue.pro>
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

#include "iodevice.h"

#if defined(GMIO_HAVE_SYS_TYPES_H) && defined(GMIO_HAVE_SYS_STAT_H)
#  include <sys/types.h>
#  include <sys/stat.h>

// gmio_stat_t: type alias on the stat structure
// GMIO_FSTAT_FUNC_NAME: alias on the fstat() function
#  if defined(GMIO_HAVE_WIN__FSTAT64)
typedef struct __stat64 gmio_stat_t;
#    define GMIO_FSTAT_FUNC_NAME _fstat64
#  elif defined(GMIO_HAVE_POSIX_FSTAT64)
typedef struct stat64 gmio_stat_t;
#    define GMIO_FSTAT_FUNC_NAME fstat64
#  else
typedef struct stat gmio_stat_t;
#    define GMIO_FSTAT_FUNC_NAME fstat
#  endif

// Define wrapper around the fstat() function
inline int gmio_fstat(int fd, gmio_stat_t* buf)
{
    return GMIO_FSTAT_FUNC_NAME(fd, buf);
}

#endif // GMIO_HAVE_SYS_TYPES_H && GMIO_HAVE_SYS_STAT_H

namespace gmio {

FuncReadData FILE_funcReadData(FILE *file)
{
    return [=](uint8_t* buff, uint64_t size, ReadState* state) {
        const size_t count = std::fread(buff, 1, size, file);
        if (state != nullptr) {
            if (std::feof(file) != 0)
                *state = ReadState::Eof;
            else if (std::ferror(file) != 0)
                *state = ReadState::Error;
            else
                *state = ReadState::Good;
        }
        return count;
    };
}

FuncWriteData FILE_funcWriteData(FILE *file)
{
    return [=](const uint8_t* buff, uint64_t size) {
        return std::fwrite(buff, 1, size, file);
    };
}

uint64_t FILE_size(FILE* file)
{
#if defined(GMIO_HAVE_SYS_TYPES_H) \
    && defined(GMIO_HAVE_SYS_STAT_H) \
    && defined(GMIO_HAVE_POSIX_FILENO)

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
    const long old_pos = std::ftell(file);

    std::fseek(file, 0L, SEEK_END);
    size = std::ftell(file);
    std::fseek(file, old_pos, SEEK_SET);

    return size;

#endif
}


FuncReadData istream_funcReadData(std::istream* stream)
{
    return [=](uint8_t* buff, uint64_t size, ReadState* state) {
        stream->read(reinterpret_cast<char*>(buff), size);
        if (state != nullptr) {
            if (stream->eof())
                *state = ReadState::Eof;
            else if (stream->fail() || stream->bad())
                *state = ReadState::Error;
            else
                *state = ReadState::Good;
        }
        return stream->gcount();
    };
}

FuncWriteData ostream_funcWriteData(std::ostream* stream)
{
    return [=](const uint8_t* buff, uint64_t size) {
        stream->write(reinterpret_cast<const char*>(buff), size);
        return size;
    };
}

} //namespace gmio
