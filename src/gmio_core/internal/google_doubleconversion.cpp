/****************************************************************************
** Copyright (c) 2016, Fougue Ltd. <http://www.fougue.pro>
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
**     1. Redistributions of source code must retain the above copyright
**        notice, this list of conditions and the following disclaimer.
**
**     2. Redistributions in binary form must reproduce the above
**        copyright notice, this list of conditions and the following
**        disclaimer in the documentation and/or other materials provided
**        with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
****************************************************************************/

#include "google_doubleconversion.h"

#if GMIO_STR2FLOAT_LIB == GMIO_STR2FLOAT_LIB_DOUBLE_CONVERSION \
    || GMIO_FLOAT2STR_LIB == GMIO_FLOAT2STR_LIB_DOUBLE_CONVERSION

#include "c99_math_compat.h"
#include "string_ascii_utils.h"
#include "../../3rdparty/double-conversion/double-conversion.h"

#include <limits>

inline float gmio_snanf()
{
    return std::numeric_limits<float>::signaling_NaN();
}

inline float gmio_inff()
{
    return std::numeric_limits<float>::infinity();
}

float gmio_str2float_googledoubleconversion(const char* num, size_t numlen)
{
    // Important note: implementation adapted from Qt5 source code
    //     qtbase/src/corelib/tools/qlocale_tools.cpp
    //      function asciiToDouble()
    // License for gmio_str2float_googledoubleconversion():
    /*
    **
    ** Copyright (C) 2016 The Qt Company Ltd.
    ** Copyright (C) 2016 Intel Corporation.
    ** Contact: https://www.qt.io/licensing/
    **
    ** This file is part of the QtCore module of the Qt Toolkit.
    **
    **  GNU Lesser General Public License Usage
    ** Alternatively, this file may be used under the terms of the GNU Lesser
    ** General Public License version 3 as published by the Free Software
    ** Foundation and appearing in the file LICENSE.LGPL3 included in the
    ** packaging of this file. Please review the following information to
    ** ensure the GNU Lesser General Public License version 3 requirements
    ** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
    **
    */

    bool ok = false;
    int processed = 0;
    float f = 0.f;
    static const int conv_flags =
            double_conversion::StringToDoubleConverter::NO_FLAGS;

    if (*num == '\0') {
        ok = false;
        processed = 0;
        return 0.f;
    }
    ok = true;

    // We have to catch NaN before because we need NaN as marker for "garbage"
    // in the libdouble-conversion case and, in contrast to libdouble-conversion
    // or sscanf, we don't allow "-nan" or "+nan"
    if (gmio_ascii_stricmp(num, "nan") == 0) {
        processed = 3;
        return gmio_snanf();
    }
    else if ((num[0] == '-' || num[0] == '+')
             && gmio_ascii_stricmp(num + 1, "nan") == 0)
    {
        processed = 0;
        ok = false;
        return 0.f;
    }

    // Infinity values are implementation defined in the sscanf case. In the
    // libdouble-conversion case we need infinity as overflow marker
    if (gmio_ascii_stricmp(num, "+inf") == 0) {
        processed = 4;
        return gmio_inff();
    } else if (gmio_ascii_stricmp(num, "inf") == 0) {
        processed = 3;
        return gmio_inff();
    } else if (gmio_ascii_stricmp(num, "-inf") == 0) {
        processed = 4;
        return -gmio_inff();
    }

    static const double_conversion::StringToDoubleConverter conv(
                conv_flags, 0.0, gmio_snanf(), 0, 0);
    f = conv.StringToFloat(num, static_cast<int>(numlen), &processed);
    if (!gmio_isfinite(f)) {
        ok = false;
        if (gmio_isnan(f)) {
            // Garbage found. We don't accept it and return 0
            processed = 0;
            return 0.f;
        } else {
            // Overflow. That's not OK, but we still return infinity
            return f;
        }
    }

#if 0
    // Check if underflow has occurred.
    if (isZero(d)) {
        for (int i = 0; i < processed; ++i) {
            if (num[i] >= '1' && num[i] <= '9') {
                // if a digit before any 'e' is not 0, then a non-zero number was intended.
                ok = false;
                return 0.0;
            } else if (num[i] == 'e') {
                break;
            }
        }
    }
#endif
    return f;
}

int gmio_float2str_googledoubleconversion(
        float value,
        char *buff,
        size_t bufflen,
        gmio_float_text_format textformat,
        uint8_t prec)
{
    static const int flags =
            double_conversion::DoubleToStringConverter::UNIQUE_ZERO
            | double_conversion::DoubleToStringConverter::EMIT_POSITIVE_EXPONENT_SIGN;
    const bool lowercase_exp_char =
            textformat == GMIO_FLOAT_TEXT_FORMAT_DECIMAL_LOWERCASE
            || textformat == GMIO_FLOAT_TEXT_FORMAT_SCIENTIFIC_LOWERCASE
            || textformat == GMIO_FLOAT_TEXT_FORMAT_SHORTEST_LOWERCASE;
    const char exp_char = lowercase_exp_char ? 'e' : 'E';
    const double_conversion::DoubleToStringConverter conv(
                flags, "Infinity", "NaN", exp_char, -6, 21, 6, 0);
    double_conversion::StringBuilder result_builder(
                buff, static_cast<int>(bufflen));
    switch (textformat) {
    case GMIO_FLOAT_TEXT_FORMAT_DECIMAL_LOWERCASE:
    case GMIO_FLOAT_TEXT_FORMAT_DECIMAL_UPPERCASE:
        conv.ToFixed(value, prec, &result_builder);
        break;
    case GMIO_FLOAT_TEXT_FORMAT_SCIENTIFIC_LOWERCASE:
    case GMIO_FLOAT_TEXT_FORMAT_SCIENTIFIC_UPPERCASE:
        conv.ToExponential(value, prec, &result_builder);
        break;
    case GMIO_FLOAT_TEXT_FORMAT_SHORTEST_LOWERCASE:
    case GMIO_FLOAT_TEXT_FORMAT_SHORTEST_UPPERCASE:
        conv.ToPrecision(value, prec, &result_builder);
        break;
    }
    return result_builder.position();
}

#endif /* GMIO_STR2FLOAT_LIB == LIB_GOOGLE_DOUBLE_CONVERSION */
