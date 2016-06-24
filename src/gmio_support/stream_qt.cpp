/****************************************************************************
** gmio
** Copyright Fougue (24 Jun. 2016)
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
    struct gmio_stream stream = {0};
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
