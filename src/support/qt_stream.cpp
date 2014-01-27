#include "qt_stream.h"

#include <QtCore/QFile>
#include <QtCore/QIODevice>

static foug_bool_t foug_stream_qiodevice_at_end(void* cookie)
{
  return static_cast<QIODevice*>(cookie)->atEnd();
}

static int32_t foug_stream_qiodevice_error(void* cookie)
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

static size_t foug_stream_qiodevice_read(void* cookie,
                                         void* ptr,
                                         size_t item_size,
                                         size_t item_count)
{
  QIODevice* device = static_cast<QIODevice*>(cookie);
  const qint64 c = device->read(static_cast<char*>(ptr), item_size * item_count);
  return c / item_size;
}

static size_t foug_stream_qiodevice_write(void* cookie,
                                          const void* ptr,
                                          size_t item_size,
                                          size_t item_count)
{
  QIODevice* device = static_cast<QIODevice*>(cookie);
  const qint64 c = device->write(static_cast<const char*>(ptr), item_size * item_count);
  return c / item_size;
}

void foug_stream_set_qiodevice(foug_stream_t* stream, QIODevice* device)
{
  stream->cookie = device;
  stream->at_end_func = foug_stream_qiodevice_at_end;
  stream->error_func = foug_stream_qiodevice_error;
  stream->read_func = foug_stream_qiodevice_read;
  stream->write_func = foug_stream_qiodevice_write;
}
