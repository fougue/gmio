/* Copyright (C) 2002-2012 Nikolaus Gebhardt
 * This file is part of the "Irrlicht Engine" and the "irrXML" project.
 * For conditions of distribution and use, see copyright notice in irrlicht.h
 * and irrXML.h
 */

/* Adapted to ISO-C90 */

#ifndef GMIO_INTERNAL_FAST_ATOF_H
#define GMIO_INTERNAL_FAST_ATOF_H

#include "../global.h"
#include "string_utils.h"

#include <float.h>
#include <limits.h>
#include <math.h>

GMIO_INLINE gmio_bool_t is_local_decimal_point(char in)
{
    /*! Selection of characters which count as decimal point in fast_atof
     * TODO: This should probably also be used in irr::core::string, but
     * the float-to-string code used there has to be rewritten first.
     */
    return in == '.';
}

/* we write [17] here instead of [] to work around a swig bug */
static const float fast_atof_table[17] = {
    0.f,
    0.1f,
    0.01f,
    0.001f,
    0.0001f,
    0.00001f,
    0.000001f,
    0.0000001f,
    0.00000001f,
    0.000000001f,
    0.0000000001f,
    0.00000000001f,
    0.000000000001f,
    0.0000000000001f,
    0.00000000000001f,
    0.000000000000001f,
    0.0000000000000001f
};

/*! Convert a simple string of base 10 digits into an unsigned 32 bit integer.
 *
 * \param[in] in: The string of digits to convert. No leading chars are
 * allowed, only digits 0 to 9. Parsing stops at the first non-digit.
 * \param[out] out: (optional) If provided, it will be set to point at the
 * first character not used in the calculation.
 * \return The unsigned integer value of the digits. If the string specifies
 * too many digits to encode in an uint32_t then INT_MAX will be returned.
 */
GMIO_INLINE uint32_t strtoul10(const char* in, const char** out)
{
    gmio_bool_t overflow=GMIO_FALSE;
    uint32_t unsignedValue = 0;

    while ( gmio_ascii_isdigit(*in) )
    {
        const uint32_t tmp = ( unsignedValue * 10 ) + ( *in - '0' );
        if (tmp<unsignedValue)
        {
            unsignedValue=(uint32_t)0xffffffff;
            overflow=GMIO_TRUE;
        }
        if (!overflow)
            unsignedValue = tmp;
        ++in;
    }
    if (out)
        *out = in;
    return unsignedValue;
}

/*! Convert a simple string of base 10 digits into a signed 32 bit integer.
 *
 * \param[in] in: The string of digits to convert. Only a leading - or +
 * followed by digits 0 to 9 will be considered. Parsing stops at the first
 * non-digit.
 * \param[out] out: (optional) If provided, it will be set to point at the
 * first character not used in the calculation.
 * \return The signed integer value of the digits. If the string specifies
 * too many digits to encode in an int32_t then +INT_MAX or -INT_MAX will be
 * returned.
 */
GMIO_INLINE int32_t strtol10(const char* in, const char** out)
{
    const gmio_bool_t negative = ('-' == *in);
    uint32_t unsignedValue = 0;

    if (negative || ('+' == *in))
        ++in;
    unsignedValue = strtoul10(in,out);
    if (unsignedValue > (uint32_t)INT_MAX)
    {
        if (negative)
            return (int32_t)INT_MIN;
        else
            return (int32_t)INT_MAX;
    }
    else
    {
        if (negative)
            return -((int32_t)unsignedValue);
        else
            return (int32_t)unsignedValue;
    }
}

/*! Convert a hex-encoded character to an unsigned integer.
 *
 * \param[in] in The digit to convert. Only digits 0 to 9 and chars A-F,a-f
 * will be considered.
 * \return The unsigned integer value of the digit. 0xffffffff if the input is
 * not hex
 */
GMIO_INLINE uint32_t ctoul16(char in)
{
    if (in >= '0' && in <= '9')
        return in - '0';
    else if (in >= 'a' && in <= 'f')
        return 10u + in - 'a';
    else if (in >= 'A' && in <= 'F')
        return 10u + in - 'A';
    else
        return 0xffffffff;
}

/*! Convert a simple string of base 16 digits into an unsigned 32 bit integer.
 *
 * \param[in] in: The string of digits to convert. No leading chars are
 * allowed, only digits 0 to 9 and chars A-F,a-f are allowed. Parsing stops
 * at the first illegal char.
 * \param[out] out: (optional) If provided, it will be set to point at the
 * first character not used in the calculation.
 * \return The unsigned integer value of the digits. If the string specifies
 * too many digits to encode in an uint32_t then INT_MAX will be returned.
 */
GMIO_INLINE uint32_t strtoul16(const char* in, const char** out)
{
    gmio_bool_t overflow=GMIO_FALSE;
    uint32_t unsignedValue = 0;

    for (;;)
    {
        uint32_t tmp = 0;
        if (gmio_ascii_isdigit(*in))
            tmp = (unsignedValue << 4u) + (*in - '0');
        else if ((*in >= 'A') && (*in <= 'F'))
            tmp = (unsignedValue << 4u) + (*in - 'A') + 10;
        else if ((*in >= 'a') && (*in <= 'f'))
            tmp = (unsignedValue << 4u) + (*in - 'a') + 10;
        else
            break;
        if (tmp<unsignedValue)
        {
            unsignedValue=(uint32_t)INT_MAX;
            overflow=GMIO_TRUE;
        }
        if (!overflow)
            unsignedValue = tmp;
        ++in;
    }
    if (out)
        *out = in;
    return unsignedValue;
}
/*! Convert a simple string of base 8 digits into an unsigned 32 bit integer.
 *
 * \param[in] in The string of digits to convert. No leading chars are
 * allowed, only digits 0 to 7 are allowed. Parsing stops at the first illegal
 * char.
 * \param[out] out (optional) If provided, it will be set to point at the
 * first character not used in the calculation.
 * \return The unsigned integer value of the digits. If the string specifies
 * too many digits to encode in an uint32_t then INT_MAX will be returned.
 */
GMIO_INLINE uint32_t strtoul8(const char* in, const char** out)
{
    gmio_bool_t overflow=GMIO_FALSE;
    uint32_t unsignedValue = 0;
    for (;;)
    {
        uint32_t tmp = 0;
        if ((*in >= '0') && (*in <= '7'))
            tmp = (unsignedValue << 3u) + (*in - '0');
        else
            break;
        if (tmp<unsignedValue)
        {
            unsignedValue=(uint32_t)INT_MAX;
            overflow=GMIO_TRUE;
        }
        if (!overflow)
            unsignedValue = tmp;
        ++in;
    }
    if (out)
        *out = in;
    return unsignedValue;
}

/*! Convert a C-style prefixed string (hex, oct, integer) into an unsigned
 *  32 bit integer.
 *
 * \param[in] in The string of digits to convert. If string starts with 0x the
 * hex parser is used, if only leading 0 is used, oct parser is used. In all
 * other cases, the usual unsigned parser is used.
 * \param[out] out (optional) If provided, it will be set to point at the
 * first character not used in the calculation.
 * \return The unsigned integer value of the digits. If the string specifies
 * too many digits to encode in an uint32_t then INT_MAX will be returned.
 */
GMIO_INLINE uint32_t strtoul_prefix(const char* in, const char** out)
{
    if ('0'==in[0])
        return ('x'==in[1] ? strtoul16(in+2,out) : strtoul8(in+1,out));
    return strtoul10(in,out);
}

/*! Converts a sequence of digits into a whole positive floating point value.
 *
 * Only digits 0 to 9 are parsed. Parsing stops at any other character,
 * including sign characters or a decimal point.
 * \param in: the sequence of digits to convert.
 * \param out: (optional) will be set to point at the first non-converted
 * character.
 * \return The whole positive floating point representation of the digit
 * sequence.
 */
GMIO_INLINE gmio_float32_t strtof10(const char* in, const char** out)
{
    const uint32_t MAX_SAFE_U32_VALUE = UINT_MAX / 10 - 10;
    uint32_t intValue = 0;
    gmio_float32_t floatValue = 0.f;

    /* Use integer arithmetic for as long as possible, for speed
     * and precision. */
    for (; gmio_ascii_isdigit(*in) && intValue < MAX_SAFE_U32_VALUE; ++in)
        intValue = (intValue * 10) + (*in - '0');
    floatValue = (gmio_float32_t)intValue;
    /* If there are any digits left to parse, then we need to use
     * floating point arithmetic from here. */
    for (; gmio_ascii_isdigit(*in) && floatValue <= FLT_MAX; ++in)
        floatValue = (floatValue * 10) + (*in - '0');
    if (out)
        *out = in;
    return floatValue;
}

/*! Provides a fast function for converting a string into a float.
 *
 * This is not guaranteed to be as accurate as atof(), but is
 * approximately 6 to 8 times as fast.
 * \param[in] in The string to convert.
 * \param[out] result The resultant float will be written here.
 * \return Pointer to the first character in the string that wasn't used
 * to create the float value.
 */
GMIO_INLINE const char* fast_atof_move(
        const char* in, gmio_float32_t* result)
{
    const gmio_bool_t negative = ('-' == *in);
    gmio_float32_t value = 0.f;

    /* Please run the regression test when making any modifications to this
     * function. */
    *result = 0.f;
    if (negative || ('+'==*in))
        ++in;
    value = strtof10(in, &in);
    if ( is_local_decimal_point(*in) )
    {
        const char* afterDecimal = ++in;
        const gmio_float32_t decimal = strtof10(in, &afterDecimal);
        value += decimal * fast_atof_table[afterDecimal - in];
        in = afterDecimal;
    }
    if ('e' == *in || 'E' == *in)
    {
        ++in;
        /* Assume that the exponent is a whole number.
         * strtol10() will deal with both + and - signs,
         * but calculate as gmio_float32_t to prevent overflow at FLT_MAX */
        value *=
#ifdef GMIO_HAVE_POWF_FUNC
                powf(10.f, (gmio_float32_t)strtol10(in, &in));
#else
                (gmio_float32_t)pow(10., (gmio_float64_t)strtol10(in, &in));
#endif
    }
    *result = negative?-value:value;
    return in;
}

/*! Convert a string to a floating point number
 *
 * \param floatAsString The string to convert.
 * \param out Optional pointer to the first character in the string that
 * wasn't used to create the float value.
 * \result Float value parsed from the input string
 */
GMIO_INLINE float fast_atof(const char* floatAsString, const char** out)
{
    float ret;
    if (out)
        *out=fast_atof_move(floatAsString, &ret);
    else
        fast_atof_move(floatAsString, &ret);
    return ret;
}

#endif /* GMIO_INTERNAL_FAST_ATOF_H */
