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

#ifndef GMIO_TESTS_CORE_UTILS_H
#define GMIO_TESTS_CORE_UTILS_H

#include "../src/gmio_core/global.h"
#include "../src/gmio_core/internal/string_ascii_utils.h"

#include <stddef.h>

/*! Trim whitespaces in string \p str from end */
void gmio_string_trim_from_end(char* str, size_t len);

#endif /* GMIO_TESTS_CORE_UTILS_H */
