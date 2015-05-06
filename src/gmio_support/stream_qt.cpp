/****************************************************************************
** GeomIO Library
** Copyright Fougue (2 Mar. 2015)
** contact@fougsys.fr
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

QT_USE_NAMESPACE

static gmio_bool_t gmio_stream_qiodevice_at_end(void* cookie)
{
    return static_cast<QIODevice*>(cookie)->atEnd();
}

static int gmio_stream_qiodevice_error(void* cookie)
{
    const QIODevice* device = static_cast<QIODevice*>(cookie);
    const QFile* file = dynamic_cast<const QFile*>(device);
    if (file != NULL) {
        return file->error();
    }
    else {
        const QString err_str = static_cast<QIODevice*>(cookie)->errorString();
        return !err_str.isEmpty() ? 1 : 0;
    }
    return 0;
}

static size_t gmio_stream_qiodevice_read(
        void* cookie, void* ptr, size_t item_size, size_t item_count)
{
    QIODevice* device = static_cast<QIODevice*>(cookie);
    const qint64 c = device->read(static_cast<char*>(ptr), item_size * item_count);
    return c / item_size;
}

static size_t gmio_stream_qiodevice_write(
        void* cookie, const void* ptr, size_t item_size, size_t item_count)
{
    QIODevice* device = static_cast<QIODevice*>(cookie);
    const qint64 c = device->write(
                static_cast<const char*>(ptr), item_size * item_count);
    return c / item_size;
}

static size_t gmio_stream_qiodevice_size(void* cookie)
{
    QIODevice* device = static_cast<QIODevice*>(cookie);
    return device->size();
}

static void gmio_stream_qiodevice_rewind(void* cookie)
{
    QIODevice* device = static_cast<QIODevice*>(cookie);
    device->seek(0);
}

gmio_stream_t gmio_stream_qiodevice(QIODevice* device)
{
    gmio_stream_t stream = { 0 };
    stream.cookie = device;
    stream.at_end_func = gmio_stream_qiodevice_at_end;
    stream.error_func = gmio_stream_qiodevice_error;
    stream.read_func = gmio_stream_qiodevice_read;
    stream.write_func = gmio_stream_qiodevice_write;
    stream.size_func = gmio_stream_qiodevice_size;
    stream.rewind_func = gmio_stream_qiodevice_rewind;
    return stream;
}
