/****************************************************************************
** Copyright (c) 2017, Fougue Ltd. <http://www.fougue.pro>
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

#include "string_ascii_utils.h"
#include "../../3rdparty/double-conversion/double-conversion.h"

#include <limits>

namespace gmio {

namespace {

template<typename T> T gmio_snan()
{ return std::numeric_limits<T>::signaling_NaN(); }

template<typename T> T gmio_inf()
{ return std::numeric_limits<T>::infinity(); }

template<typename T> struct FloatTraits {};
template<> struct FloatTraits<float> { static float zero() { return 0.f; } };
template<> struct FloatTraits<double> { static double zero() { return 0.; } };

template<typename T>
T GDC_str2floatGeneric(const char* num, size_t numlen)
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
    T f = FloatTraits<T>::zero();
    static const int conv_flags =
            double_conversion::StringToDoubleConverter::NO_FLAGS;

    if (*num == '\0') {
        ok = false;
        processed = 0;
        return FloatTraits<T>::zero();
    }
    ok = true;

    // We have to catch NaN before because we need NaN as marker for "garbage"
    // in the libdouble-conversion case and, in contrast to libdouble-conversion
    // or sscanf, we don't allow "-nan" or "+nan"
    if (ascii_stricmp(num, "nan") == 0) {
        processed = 3;
        return gmio_snan<T>();
    }
    else if ((num[0] == '-' || num[0] == '+')
             && ascii_stricmp(num + 1, "nan") == 0)
    {
        processed = 0;
        ok = false;
        return FloatTraits<T>::zero();
    }

    // Infinity values are implementation defined in the sscanf case. In the
    // libdouble-conversion case we need infinity as overflow marker
    if (ascii_stricmp(num, "+inf") == 0) {
        processed = 4;
        return gmio_inf<T>();
    } else if (ascii_stricmp(num, "inf") == 0) {
        processed = 3;
        return gmio_inf<T>();
    } else if (ascii_stricmp(num, "-inf") == 0) {
        processed = 4;
        return -gmio_inf<T>();
    }

    static const double_conversion::StringToDoubleConverter conv(
                conv_flags, 0., gmio_snan<T>(), nullptr, nullptr);
    f = conv.StringToFloat(num, static_cast<int>(numlen), &processed);
    if (!std::isfinite(f)) {
        ok = false;
        if (std::isnan(f)) {
            // Garbage found. We don't accept it and return 0
            processed = 0;
            return FloatTraits<T>::zero();
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

template<typename T>
int GDC_float2strGeneric(
        T value,
        char *buff,
        size_t bufflen,
        FloatTextFormat textformat,
        uint8_t prec)
{
    static const int flags =
            double_conversion::DoubleToStringConverter::UNIQUE_ZERO
            | double_conversion::DoubleToStringConverter::EMIT_POSITIVE_EXPONENT_SIGN;
    const bool lowercase_exp_char =
            textformat == FloatTextFormat::DecimalLowercase
            || textformat == FloatTextFormat::ScientificLowercase
            || textformat == FloatTextFormat::ShortestLowercase;
    const char exp_char = lowercase_exp_char ? 'e' : 'E';
    const double_conversion::DoubleToStringConverter conv(
                flags, "Infinity", "NaN", exp_char, -6, 21, 6, 0);
    double_conversion::StringBuilder result_builder(
                buff, static_cast<int>(bufflen));
    switch (textformat) {
    case FloatTextFormat::DecimalLowercase:
    case FloatTextFormat::DecimalUppercase:
        conv.ToFixed(value, prec, &result_builder);
        break;
    case FloatTextFormat::ScientificLowercase:
    case FloatTextFormat::ScientificUppercase:
        conv.ToExponential(value, prec, &result_builder);
        break;
    case FloatTextFormat::ShortestLowercase:
    case FloatTextFormat::ShortestUppercase:
        conv.ToPrecision(value, prec, &result_builder);
        break;
    }
    return result_builder.position();
}

} // namespace

float GDC_str2float(const char* num, size_t numlen)
{
    return GDC_str2floatGeneric<float>(num, numlen);
}

double GDC_str2double(const char* num, size_t numlen)
{
    return GDC_str2floatGeneric<double>(num, numlen);
}

int GDC_float2str(
        float value,
        char *buff,
        size_t bufflen,
        FloatTextFormat textformat,
        uint8_t prec)
{
    return GDC_float2strGeneric<float>(value, buff, bufflen, textformat, prec);
}

int GDC_double2strGeneric(
        double value,
        char* buff,
        size_t bufflen,
        FloatTextFormat textformat,
        uint8_t prec)
{
    return GDC_float2strGeneric<double>(value, buff, bufflen, textformat, prec);
}

} // namespace gmio

#endif // GMIO_STR2FLOAT_LIB == LIB_GOOGLE_DOUBLE_CONVERSION
