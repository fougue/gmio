#include "qt4_stream.h"

#include <QtCore/QIODevice>
#include <QtCore/QtDebug>

namespace foug {

Qt4Stream::Qt4Stream(QIODevice *device)
  : m_device(device)
{
}

bool Qt4Stream::atEnd() const
{
  return m_device->atEnd();
}

bool Qt4Stream::isWritable() const
{
  return m_device != 0 ? m_device->isWritable() : false;
}

bool Qt4Stream::isReadable() const
{
  return m_device != 0 ? m_device->isReadable() : false;
}

bool Qt4Stream::isSequential() const
{
  return m_device != 0 ? m_device->isSequential() : false;
}

bool Qt4Stream::seek(int64_t pos)
{
  return m_device->seek(pos);
}

int64_t Qt4Stream::read(char *data, int64_t maxSize)
{
  return m_device->read(data, maxSize);
}

int64_t Qt4Stream::write(const char *data, int64_t maxSize)
{
  return m_device->write(data, maxSize);
}

} // namespace foug
