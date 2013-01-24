#ifndef FOUG_CPP_STREAMS_QT4_STREAM_H
#define FOUG_CPP_STREAMS_QT4_STREAM_H

#include "../abstract_stream.h"
class QIODevice;

namespace foug {

class Qt4Stream : public AbstractStream
{
public:
  Qt4Stream(QIODevice* device);

  bool atEnd() const;

  bool isWritable() const;
  bool isReadable() const;
  bool isSequential() const;

  bool seek(int64_t pos);
  int64_t read(char* data, int64_t maxSize);
  int64_t write(const char* data, int64_t maxSize);

private:
  QIODevice* m_device;
};

} // namespace foug

#endif // FOUG_CPP_STREAMS_QT4_STREAM_H
