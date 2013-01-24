#ifndef FOUG_CPP_ABSTRACT_STREAM_H
#define FOUG_CPP_ABSTRACT_STREAM_H

#include "foug_global.h"

namespace foug {

class AbstractStream
{
public:
  virtual bool atEnd() const = 0;

  virtual bool isWritable() const = 0;
  virtual bool isReadable() const = 0;
  virtual bool isSequential() const = 0;

  virtual bool seek(int64_t pos) = 0;
  virtual int64_t read(char* data, int64_t maxSize) = 0;
  virtual int64_t write(const char* data, int64_t maxSize) = 0;
};

} // namespace foug

#endif // FOUG_CPP_ABSTRACT_STREAM_H
