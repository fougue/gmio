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

#include <QtCore/QFile>
#include <QtCore/QIODevice>
#include <cstring>

QT_USE_NAMESPACE

static bool gmio_stream_qiodevice_at_end(void* cookie)
{
    return static_cast<QIODevice*>(cookie)->atEnd();
}

static int gmio_stream_qiodevice_error(void* cookie)
{
    const QIODevice* device = static_cast<QIODevice*>(cookie);
    const QFile* file = qobject_cast<const QFile*>(device);
    if (file != NULL) {
        return file->error();
    }
    else {
        const QString err_str = device->errorString();
        return !err_str.isEmpty() ? 1 : 0;
    }
    return 0;
}

static size_t gmio_stream_qiodevice_read(
        void* cookie, void* ptr, size_t item_size, size_t item_count)
{
    QIODevice* device = static_cast<QIODevice*>(cookie);
    const qint64 c = device->read(static_cast<char*>(ptr), item_size * item_count);
    return static_cast<size_t>(c / item_size);
}

static size_t gmio_stream_qiodevice_write(
        void* cookie, const void* ptr, size_t item_size, size_t item_count)
{
    QIODevice* device = static_cast<QIODevice*>(cookie);
    const qint64 c = device->write(
                static_cast<const char*>(ptr), item_size * item_count);
    return static_cast<size_t>(c / item_size);
}

static gmio_streamsize_t gmio_stream_qiodevice_size(void* cookie)
{
    QIODevice* device = static_cast<QIODevice*>(cookie);
    return device->size();
}

static int gmio_stream_qiodevice_get_pos(void* cookie, struct gmio_streampos* pos)
{
    QIODevice* device = static_cast<QIODevice*>(cookie);
    qint64 qpos = device->pos();
    std::memcpy(&pos->cookie[0], &qpos, sizeof(qint64));
    return 0;
}

static int gmio_stream_qiodevice_set_pos(
        void* cookie, const struct gmio_streampos* pos)
{
    QIODevice* device = static_cast<QIODevice*>(cookie);
    qint64 qpos;
    std::memcpy(&qpos, &pos->cookie[0], sizeof(qint64));
    if (device->seek(qpos))
        return 0;
    return -1; /* TODO: return error code */
}

struct gmio_stream gmio_stream_qiodevice(QIODevice* device)
{
    struct gmio_stream stream = {};
    stream.cookie = device;
    stream.func_at_end = gmio_stream_qiodevice_at_end;
    stream.func_error = gmio_stream_qiodevice_error;
    stream.func_read = gmio_stream_qiodevice_read;
    stream.func_write = gmio_stream_qiodevice_write;
    stream.func_size = gmio_stream_qiodevice_size;
    stream.func_get_pos = gmio_stream_qiodevice_get_pos;
    stream.func_set_pos = gmio_stream_qiodevice_set_pos;
    return stream;
}
