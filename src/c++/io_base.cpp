#include "io_base.h"

#include "abstract_stream.h"
#include "abstract_task_progress.h"

namespace foug {

namespace internal {

class IoBasePrivate
{
public:
  IoBasePrivate()
    : m_stream(0),
      m_taskProgress(0),
      m_autoDeleteStream(true),
      m_autoDeleteTaskProgress(true)
  {
  }

  AbstractStream* m_stream;
  AbstractTaskProgress* m_taskProgress;
  bool m_autoDeleteStream;
  bool m_autoDeleteTaskProgress;
};

} // namespace internal

IoBase::IoBase(AbstractStream *stream)
  : d(new internal::IoBasePrivate)
{
  d->m_stream = stream;
}

IoBase::~IoBase()
{
  if (this->autoDeleteStream() && d->m_stream != 0)
    delete d->m_stream;

  if (this->autoDeleteTaskProgress() && d->m_taskProgress != 0)
    delete d->m_taskProgress;

  delete d;
}

AbstractStream* IoBase::stream() const
{
  return d->m_stream;
}

void IoBase::setStream(AbstractStream* stream)
{
  if (this->autoDeleteStream() && d->m_stream != 0 && d->m_stream != stream)
    delete d->m_stream;
  d->m_stream = stream;
}

AbstractTaskProgress* IoBase::taskProgress() const
{
  return d->m_taskProgress;
}

void IoBase::setTaskProgress(AbstractTaskProgress* progress)
{
  if (this->autoDeleteTaskProgress() && d->m_taskProgress != 0 && d->m_taskProgress != progress)
    delete d->m_taskProgress;
  d->m_taskProgress = progress;
}

bool IoBase::autoDeleteStream() const
{
  return d->m_autoDeleteStream;
}

void IoBase::setAutoDeleteStream(bool on)
{
  d->m_autoDeleteStream = on;
}

bool IoBase::autoDeleteTaskProgress() const
{
  return d->m_autoDeleteTaskProgress;
}

void IoBase::setAutoDeleteTaskProgress(bool on) const
{
  d->m_autoDeleteTaskProgress = on;
}

} // namespace foug
