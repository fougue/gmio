#ifndef FOUG_SUPPORT_QT4_STREAM_H
#define FOUG_SUPPORT_QT4_STREAM_H

extern "C" {
#include "support_global.h"
#include "../stream.h"
}
class QIODevice;

FOUG_LIBSUPPORT_EXPORT
void foug_stream_set_qiodevice(foug_stream_t* stream, QIODevice* device);

#endif /* FOUG_SUPPORT_QT4_STREAM_H */
