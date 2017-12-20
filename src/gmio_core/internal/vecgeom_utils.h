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

#pragma once

#include "../global.h"
#include "../vecgeom.h"
#include "numeric_utils.h"
#include <limits>

namespace gmio {

//! Returns cross product of two Vec3 objects
template<typename T> Vec3<T> vec3_crossProduct(const Vec3<T>& u, const Vec3<T>& v);

//! Returns u - v
template<typename T> constexpr Vec3<T> vec3_sub(const Vec3<T>& u, const Vec3<T>& v);

//! Returns normalized Vec3 object
template<typename T> Vec3<T> vec3_normalized(const Vec3<T>& v);

//! Returns the squared length of a Vec3 object
template<typename T> constexpr T vec3_sqrLength(const Vec3<T>& v);

//! Does 'lhs' == 'rhs' using float32_ulpEquals() on coords ?
template<typename T> bool vec3_equals(
        const Vec3<T>& lhs, const Vec3<T>& rhs, uint32_t max_ulp_diff);

//
//  Implementation
//

template<typename T> Vec3<T> vec3_crossProduct(const Vec3<T>& u, const Vec3<T>& v)
{
    const T ux = u.x, uy = u.y, uz = u.z;
    const T vx = v.x, vy = v.y, vz = v.z;
    Vec3<T> n;
    n.x = uy*vz - uz*vy;
    n.y = uz*vx - ux*vz;
    n.z = ux*vy - uy*vx;
    return vec3_normalized(n);
}

template<typename T> Vec3<T> vec3_normalized(const Vec3<T>& v)
{
    const T d = std::sqrt(vec3_sqrLength(v));
    if (d > std::numeric_limits<T>::min())
        return { v.x/d, v.y/d, v.z/d};
    return v;
}

template<typename T> constexpr Vec3<T> vec3_sub(const Vec3<T>& u, const Vec3<T>& v)
{
    return { u.x - v.x, u.y - v.y, u.z - v.z };
}

template<typename T> constexpr T vec3_sqrLength(const Vec3<T>& v)
{
    return v.x*v.x + v.y*v.y + v.z*v.z;
}

template<typename T> bool vec3_equals(
        const Vec3<T>& lhs, const Vec3<T>& rhs, uint32_t max_ulp_diff)
{
    return float32_ulpEquals(lhs.x, rhs.x, max_ulp_diff)
            && float32_ulpEquals(lhs.y, rhs.y, max_ulp_diff)
            && float32_ulpEquals(lhs.z, rhs.z, max_ulp_diff);
}

} // namespace gmio
