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

#include "file_utils.h"

#include <cstring>

namespace gmio {

#if 0
struct gmio_const_string FileUtils_findBaseFilename(const char* filepath)
{
    struct gmio_const_string basefilename = {0};
    const size_t filepath_len = filepath != nullptr ? std::strlen(filepath) : 0;
    if (filepath_len == 0)
        return basefilename;

    const char* const pos_filepath_begin = filepath;
    const char* const pos_filepath_end = filepath + filepath_len;
    const char* pos_last_dot = pos_filepath_end;
    const char* it = pos_filepath_end - 1;
    while (it != pos_filepath_begin
           && *it != '/'
           && *it != '\\')
    {
        if (*it == '.' && pos_last_dot == pos_filepath_end)
            pos_last_dot = it;
        --it;
    }
    it = it != pos_filepath_begin ? it + 1 : it;
    basefilename.ptr = it;
    basefilename.len =
            filepath_len
            - (it - pos_filepath_begin) /* complete filename */
            - (pos_filepath_end - pos_last_dot); /* filename suffix */
    return basefilename;
}
#endif

} // namespace gmio
