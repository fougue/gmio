#ifndef GMIO_SUPPORT_QT_STREAM_H
#define GMIO_SUPPORT_QT_STREAM_H

#include "support_global.h"
struct gmio_stream;
class QIODevice;

/*! \brief Configures \p stream for \c QIODevice* (cookie will hold \p device)
 */
GMIO_LIBSUPPORT_EXPORT
void gmio_stream_set_qiodevice(struct gmio_stream* stream, QIODevice* device);

#endif /* GMIO_SUPPORT_QT_STREAM_H */
