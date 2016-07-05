/****************************************************************************
** gmio
** Copyright Fougue (24 Jun. 2016)
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

#include "../global.h"

const char* gmio_lc_numeric();

/*! Returns result of case-insensitive test <tt>LC_NUMERIC == "C|POSIX"</tt> */
bool gmio_lc_numeric_is_C();

/*! Checks gmio_lc_numeric_is_C(), if false sets \p *error to
 *  \c GMIO_ERROR_BAD_LC_NUMERIC*/
bool gmio_check_lc_numeric(int* error);

void gmio_lc_numeric_save();
void gmio_lc_numeric_restore();
