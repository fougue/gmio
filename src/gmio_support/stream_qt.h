/****************************************************************************
** gmio
** Copyright Fougue (2 Mar. 2015)
** contact@fougue.pro
**
** This software is a reusable library whose purpose is to provide complete
** I/O support for various CAD file formats (eg. STL)
**
** This software is governed by the CeCILL-B license under French law and
** abiding by the rules of distribution of free software.  You can  use,
** modify and/ or redistribute the software under the terms of the CeCILL-B
** license as circulated by CEA, CNRS and INRIA at the following URL
** "http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html".
****************************************************************************/

/*! \file stream_qt.h
 *  Support of Qt's QIODevice (requires at least Qt4)
 *
*  \addtogroup gmio_support
 *  @{
 */

#ifndef GMIO_SUPPORT_STREAM_QT_H
#define GMIO_SUPPORT_STREAM_QT_H

#include "support_global.h"
#include "../gmio_core/stream.h"
#include <QtCore/QtGlobal>

QT_BEGIN_NAMESPACE
class QIODevice;
QT_END_NAMESPACE

/*! Returns a gmio_stream for \c QIODevice* (cookie will hold \p device) */
GMIO_LIBSUPPORT_EXPORT
gmio_stream_t gmio_stream_qiodevice(
#ifndef DOXYGEN
        QT_PREPEND_NAMESPACE(QIODevice)* device
#else
        QIODevice* device
#endif /* !DOXYGEN */
        );

#endif /* GMIO_SUPPORT_STREAM_QT_H */
/*! @} */
