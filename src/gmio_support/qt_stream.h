/****************************************************************************
** GeomIO Library
** Copyright FougSys (2 Mar. 2015)
** contact@fougsys.fr
**
** This software is a reusable library whose purpose is to provide complete
** I/O support for various CAD file formats (eg. STL)
**
** This software is governed by the CeCILL-B license under French law and
** abiding by the rules of distribution of free software.  You can  use,
** modify and/ or redistribute the software under the terms of the CeCILL-B
** license as circulated by CEA, CNRS and INRIA at the following URL
** "http://www.cecill.info".
****************************************************************************/

/*! \file qt_stream.h
 *  Support of Qt's QIODevice (requires at least Qt4)
 */

#ifndef GMIO_SUPPORT_QT_STREAM_H
#define GMIO_SUPPORT_QT_STREAM_H

#include "support_global.h"
#include "../gmio_core/stream.h"
#include <QtCore/QtGlobal>

QT_BEGIN_NAMESPACE
class QIODevice;
QT_END_NAMESPACE

/*! Configures \p stream for \c QIODevice* (cookie will hold \p device) */
GMIO_LIBSUPPORT_EXPORT
void gmio_stream_set_qiodevice(
        gmio_stream_t* stream, QT_PREPEND_NAMESPACE(QIODevice)* device);

/*! Returns a gmio_stream for \c QIODevice* (cookie will hold \p device) */
GMIO_LIBSUPPORT_EXPORT
gmio_stream_t gmio_stream_qiodevice(QT_PREPEND_NAMESPACE(QIODevice)* device);

#endif /* GMIO_SUPPORT_QT_STREAM_H */
