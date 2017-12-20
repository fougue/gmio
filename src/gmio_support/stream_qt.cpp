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

#include <gmio_support/stream_qt.h>

#include <QtCore/QIODevice>

QT_USE_NAMESPACE

namespace gmio {

FuncReadData QIODevice_funcReadData(QIODevice* device)
{
    return [=](uint8_t* buff, uint64_t size, ReadState* state) {
        const qint64 readlen = device->read(buff, size);
        if (state != nullptr) {
            if (readlen == -1)
                return ReadState::Error;
            else if (device->atEnd())
                *state = ReadState::Eof;
            else
                *state = ReadState::Good;
        }
        return readlen;
    };
}

FuncWriteData QIODevice_funcWriteData(QIODevice* device)
{
    return [=](const uint8_t* buff, uint64_t size) {
        const qint64 writelen = device->write(buff, size);
        return writelen != -1 ? writelen : 0;
    };
}

} // namespace gmio
