#ifndef FOUG_SUPPORT_QT_STREAM_H
#define FOUG_SUPPORT_QT_STREAM_H

#include "support_global.h"
struct foug_stream;
class QIODevice;

FOUG_LIBSUPPORT_EXPORT
void foug_stream_set_qiodevice(struct foug_stream* stream, QIODevice* device);

#endif /* FOUG_SUPPORT_QT_STREAM_H */
