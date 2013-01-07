#include "abstract_task_progress.h"

#include <algorithm>
#include <cmath>

namespace foug {

namespace internal {

class AbstractTaskProgressPrivate
{
public:
  AbstractTaskProgressPrivate()
    : m_stepId(-1),
      m_value(-1.),
      m_rangeMin(-1.),
      m_rangeMax(-2.),
      m_rangeLength(0.),
      m_progressThreshold(0.01), // Notifies each percent only
      m_isTaskStopRequested(false)
  {
  }

  int m_stepId;
  double m_value;
  double m_rangeMin;
  double m_rangeMax;
  double m_rangeLength;
  double m_progressThreshold;
  bool m_isTaskStopRequested;
};

} // namespace internal

AbstractTaskProgress::AbstractTaskProgress()
  : d(new internal::AbstractTaskProgressPrivate)
{
}

AbstractTaskProgress::~AbstractTaskProgress()
{
  delete d;
}

double AbstractTaskProgress::rangeMin() const
{
  return d->m_rangeMin;
}

double AbstractTaskProgress::rangeMax() const
{
  return d->m_rangeMax;
}

void AbstractTaskProgress::setRange(double min, double max)
{
  d->m_rangeMin = min;
  d->m_rangeMax = max;
  d->m_rangeLength = max - min;
}

int AbstractTaskProgress::stepId() const
{
  return d->m_stepId;
}

void AbstractTaskProgress::setStepId(int id)
{
  d->m_stepId = id;
}

double AbstractTaskProgress::progress() const
{
  const double result = (d->m_value - d->m_rangeMin) / d->m_rangeLength;
  return std::fabs(result);
}

double AbstractTaskProgress::value() const
{
  return d->m_value;
}

void AbstractTaskProgress::setValue(double v)
{
  if (std::fabs(v - d->m_value) > std::fabs(d->m_progressThreshold * d->m_rangeLength)) {
    d->m_value = v;
    this->progressUpdateEvent();
  }
}

double AbstractTaskProgress::progressUpdateThreshold() const
{
  return d->m_progressThreshold;
}

void AbstractTaskProgress::setProgressUpdateThreshold(double v)
{
  d->m_progressThreshold = v;
}

void AbstractTaskProgress::asyncTaskStop()
{
  d->m_isTaskStopRequested = true;
}

bool AbstractTaskProgress::isTaskStopRequested() const
{
  return d->m_isTaskStopRequested;
}

void AbstractTaskProgress::taskStoppedEvent()
{
  d->m_isTaskStopRequested = false;
}

void AbstractTaskProgress::reset()
{
  d->m_stepId = -1;
  d->m_value = -1.;
  d->m_rangeMin = -1.;
  d->m_rangeMax = -2.;
  d->m_isTaskStopRequested = false;
}

} // namespace foug
